#include "ft_nm_otool.h"

struct s_sym_32			*get_sym_list_32(void *ptr_header, \
	struct symtab_command *sym_command, int8_t endian, struct s_nm_data *nm_data)
{
	struct nlist		*sym_table_entry;
	char				*sym_table_string;
	char				*sym_table_string_elem;
	struct s_sym_32		*sym_elem;
	struct s_sym_32		*sym_elem_first;
	struct s_sym_32		*sym_elem_prev;
	uint32_t			sym_command_nsyms;
	size_t				i;

	sym_elem_first = NULL;
	sym_elem_prev = NULL;
	if (endian)
	{
		sym_table_entry = ptr_header + endian_swap_int32(sym_command->symoff);
		sym_table_string = ptr_header + endian_swap_int32(sym_command->stroff);
		sym_command_nsyms = endian_swap_int32(sym_command->nsyms);
	}
	else
	{
		sym_table_entry = ptr_header + sym_command->symoff;
		sym_table_string = ptr_header + sym_command->stroff;
		sym_command_nsyms = sym_command->nsyms;
	}
	if ((size_t)((void*)sym_table_entry - (void*)ptr_header) + sizeof(*sym_table_entry) * sym_command_nsyms >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	i = 0;
	while (i < sym_command_nsyms)
	{
		sym_elem = (struct s_sym_32 *)malloc(sizeof(struct s_sym_32));
		sym_elem->elem = sym_table_entry[i];
		sym_table_string_elem = (endian) ? sym_table_string + endian_swap_int32(sym_table_entry[i].n_un.n_strx) : sym_table_string + sym_table_entry[i].n_un.n_strx;
		if ((size_t)((void*)sym_table_string - (void*)ptr_header) > nm_data->file_size)
		{
			ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
			return (NULL);
		}
		sym_elem->sym_table_string = sym_table_string_elem;
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

struct s_sect_32	*add_sect_32_to_list(struct s_nm_32 *nm_32,
	struct segment_command *seg, size_t *count_sect, \
	struct s_sect_32 *sect_last_list, int8_t endian, struct s_nm_data *nm_data, void *ptr_header)
{
	struct s_sect_32	*sect_elem;
	struct section		*sect;
	size_t				i;
	uint32_t			seg_nsects;

	i = 0;
	sect = (void *)seg + sizeof(*seg);
	sect_elem = NULL;
	seg_nsects = (endian) ? endian_swap_int32(seg->nsects) : seg->nsects;
	if ((size_t)((void*)sect - (void*)ptr_header) + sizeof(*sect) * seg_nsects >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	while(i < seg_nsects)
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

struct s_nm_32		*get_nm_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	size_t						i;
	size_t						count_sect;
	struct s_sect_32			*sect_last_list;
	struct mach_header			*header;
	struct load_command			*lc;
	struct s_nm_32				*nm_32;
	struct symtab_command		*sym_command;
	struct segment_command_64	*seg;
	uint32_t					header_ncmds;
	uint32_t					lc_cmd;

	if (sizeof(*header) + sizeof(*lc) >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	header = (struct mach_header *)ptr_header;
	header_ncmds = (endian) ? endian_swap_int32(header->ncmds) : header->ncmds;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	count_sect = 1;
	i = 0;
	sect_last_list = NULL;
	nm_32 = (struct s_nm_32*)malloc(sizeof(struct s_nm_32));
	nm_32->sym_list = NULL;
	nm_32->sect_list = NULL;
	if (sizeof(*lc) * ((header_ncmds == 0) ? 0 : header_ncmds - 1) + ((sizeof(*seg) > sizeof(*sym_command)) ? sizeof(*seg) : sizeof(*sym_command)) >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	while (i++ < header_ncmds)
	{
		lc_cmd = (endian) ? endian_swap_int32(lc->cmd) : lc->cmd;
		if (lc_cmd == LC_SYMTAB)
		{
			sym_command = (struct symtab_command *)lc;
			nm_32->sym_list = get_sym_list_32(ptr_header, sym_command, endian, nm_data);
			nm_32->sym_list_size = (endian) ? endian_swap_int32(sym_command->nsyms) : sym_command->nsyms;
		}
		if (lc_cmd == LC_SEGMENT)
		{
			sect_last_list = add_sect_32_to_list(nm_32, (struct segment_command *)lc, &count_sect, sect_last_list, endian, nm_data, ptr_header);
		}
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) : (void *)lc + lc->cmdsize;
	}
	return (nm_32);
}
