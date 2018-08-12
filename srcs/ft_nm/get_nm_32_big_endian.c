#include "ft_nm_otool.h"

struct s_sym_32			*get_sym_list_32_big_endian(void *ptr_header, \
	struct symtab_command *sym_command)
{
	struct nlist		*sym_table_entry;
	char				*sym_table_string;
	struct s_sym_32		*sym_elem;
	struct s_sym_32		*sym_elem_first;
	struct s_sym_32		*sym_elem_prev;
	size_t				i;

	sym_elem_first = NULL;
	sym_elem_prev = NULL;
	sym_table_entry = ptr_header + sym_command->symoff;
	sym_table_string = ptr_header + sym_command->stroff;
	i = 0;
	while (i < sym_command->nsyms)
	{
		sym_elem = (struct s_sym_32 *)malloc(sizeof(struct s_sym_32));
		sym_elem->elem = sym_table_entry[i];
		sym_elem->sym_table_string = sym_table_string + sym_table_entry[i].n_un.n_strx;
		sym_elem->order = i;
		if (i == 0)
			sym_elem_first = sym_elem;
		if (sym_elem_prev)
			sym_elem_prev->next = sym_elem;
		sym_elem_prev = sym_elem;
		i++;
	}
	if (i > 0)
		sym_elem->next = NULL;
	return (sym_elem_first);
}

struct s_sect_32	*add_sect_32_to_list_big_endian(struct s_nm_32 *nm_32, \
	struct segment_command *seg, size_t *count_sect, \
	struct s_sect_32 *sect_last_list)
{
	struct s_sect_32	*sect_elem;
	struct section		*sect;
	size_t				i;

	i = 0;
	sect = (void *)seg + sizeof(*seg);
	sect_elem = NULL;
	while(i < seg->nsects)
	{
		sect_elem = (struct s_sect_32 *)malloc(sizeof(struct s_sect_32));
		sect_elem->elem = sect[i];
		sect_elem->ordinal = *count_sect;
		if (sect_last_list)
			sect_last_list->next = sect_elem;
		sect_last_list = sect_elem;
		if (!nm_32->sect_list)
			nm_32->sect_list = sect_elem;
		(*count_sect)++;
		i++;
	}
	if (sect_elem)
		sect_elem->next = NULL;
	if (i == 0)
		return (sect_last_list);
	return (sect_elem);
}

struct s_nm_32		*get_nm_32_big_endian(void *ptr_header)
{
	size_t						i;
	size_t						count_sect;
	struct s_sect_32			*sect_last_list;
	struct mach_header			*header;
	struct load_command			*lc;
	struct s_nm_32				*nm_32;
	struct symtab_command		*sym_command;

	header = (struct mach_header *)ptr_header;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	count_sect = 1;
	i = 0;
	sect_last_list = NULL;
	nm_32 = (struct s_nm_32*)malloc(sizeof(struct s_nm_32));
	nm_32->sym_list = NULL;
	nm_32->sect_list = NULL;
	while (i++ < header->ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym_command = (struct symtab_command *)lc;
			nm_32->sym_list = get_sym_list_32_big_endian(ptr_header, sym_command);
			nm_32->sym_list_size = sym_command->nsyms;
		}
		if (lc->cmd == LC_SEGMENT)
		{
			sect_last_list = add_sect_32_to_list_big_endian(nm_32, (struct segment_command *)lc, &count_sect, sect_last_list);
		}
		lc = (void *)lc + lc->cmdsize;
	}
	return (nm_32);
}
