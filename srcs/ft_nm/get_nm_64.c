#include "ft_nm_otool.h"

struct s_sym_64			*get_sym_list_64(char *content, struct symtab_command *sym_command)
{
	struct nlist_64		*sym_table_entry;
	char				*sym_table_string;
	struct s_sym_64		*sym_elem;
	struct s_sym_64		*sym_elem_first;
	struct s_sym_64		*sym_elem_prev;
	size_t				i;

	sym_elem_first = NULL;
	sym_elem_prev = NULL;
	sym_table_entry = (void *)content + sym_command->symoff;
	sym_table_string = (void *)content + sym_command->stroff;
	i = 0;
	while (i < sym_command->nsyms)
	{
		sym_elem = (struct s_sym_64 *)malloc(sizeof(struct s_sym_64));
		sym_elem->current = sym_table_entry[i];
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

struct s_sect_64	*add_sect_64_to_list(struct s_nm_64 *nm_64, struct segment_command_64 *seg, size_t *count_sect, struct s_sect_64 *sect_last_list)
{
	struct s_sect_64	*sect_elem;
	struct section_64	*sect;
	size_t				i;

	i = 0;
	sect = (void *)seg + sizeof(*seg);
	while(i < seg->nsects)
	{
		sect_elem = (struct s_sect_64 *)malloc(sizeof(struct s_sect_64));
		sect_elem->current = sect[i];
		sect_elem->ordinal = *count_sect;
		if (sect_last_list)
			sect_last_list->next = sect_elem;
		sect_last_list = sect_elem;
		if (!nm_64->sect_list)
			nm_64->sect_list = sect_elem;
		(*count_sect)++;
		i++;
	}
	if (sect_elem)
		sect_elem->next = NULL;
	if (i == 0)
		return (sect_last_list);
	return (sect_elem);
}

struct s_nm_64		*get_nm_64(char *content)
{
	size_t						i;
	size_t						count_sect;
	struct s_sect_64			*sect_last_list;
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct s_nm_64				*nm_64;

	header = (struct mach_header_64 *)content;
	lc = (void *)content + sizeof(*header);
	count_sect = 1;
	i = 0;
	sect_last_list = NULL;
	nm_64 = (struct s_nm_64*)malloc(sizeof(struct s_nm_64));
	nm_64->sym_list = NULL;
	nm_64->sect_list = NULL;
	while (i++ < header->ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
			nm_64->sym_list = get_sym_list_64(content, (struct symtab_command *)lc);
		if (lc->cmd == LC_SEGMENT_64)
			sect_last_list = add_sect_64_to_list(nm_64, (struct segment_command_64 *)lc, &count_sect, sect_last_list);
		lc = (void *)lc + lc->cmdsize;
	}
	return (nm_64);
}
