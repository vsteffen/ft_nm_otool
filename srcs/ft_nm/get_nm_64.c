/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_nm_64.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/30 18:46:46 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/30 18:46:47 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

struct s_sym_64			*get_sym_list_64(void *ptr_header, \
	struct symtab_command *sym_command, int8_t endian, struct s_nm_data *nm_data)
{
	struct nlist_64		*sym_table_entry;
	char				*sym_table_string;
	char				*sym_table_string_elem;
	struct s_sym_64		*sym_elem;
	struct s_sym_64		*sym_elem_first;
	struct s_sym_64		*sym_elem_prev;
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
	if ((size_t)((void*)sym_table_entry - (void*)ptr_header) + sizeof(*sym_command) + sizeof(*sym_table_entry) * sym_command_nsyms >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	i = 0;
	while (i < sym_command_nsyms)
	{
		sym_elem = (struct s_sym_64 *)malloc(sizeof(struct s_sym_64));
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

struct s_sect_64	*add_sect_64_to_list(struct s_nm_64 *nm_64, \
	struct segment_command_64 *seg, size_t *count_sect, \
	struct s_sect_64 *sect_last_list, int8_t endian, struct s_nm_data *nm_data, void *ptr_header)
{
	struct s_sect_64	*sect_elem;
	struct section_64	*sect;
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
	while (i < seg_nsects)
	{
		sect_elem = (struct s_sect_64 *)malloc(sizeof(struct s_sect_64));
		sect_elem->elem = sect[i];
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

void				init_get_nm_64(struct s_get_nm_64 *get, void *ptr_header, \
	int8_t endian)
{
	get->header = (struct mach_header_64 *)ptr_header;
	get->header_ncmds = (endian) ? endian_swap_int32(get->header->ncmds) \
		: get->header->ncmds;
	get->lc = (struct load_command *)(ptr_header + sizeof(*(get->header)));
	get->count_sect = 1;
	get->i = 0;
	get->sect_last_list = NULL;
	get->nm_64 = (struct s_nm_64*)malloc(sizeof(struct s_nm_64));
	get->nm_64->sym_list = NULL;
	get->nm_64->sect_list = NULL;
}

int8_t				verif_corrupted_nm_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data, struct s_get_nm_64 *get)
{
	if (sizeof(*(get->header)) + sizeof(*(get->lc)) >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (1);
	}
	init_get_nm_64(get, ptr_header, endian);
	if (sizeof(*(get->lc)) * ((get->header_ncmds == 0) ? 0 : get->header_ncmds \
		- 1) + ((sizeof(*(get->seg)) > sizeof(*(get->sym_command))) \
		? sizeof(*(get->seg)) : sizeof(*(get->sym_command))) \
		>= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (1);
	}
	return (0);
}

struct s_nm_64		*get_nm_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_get_nm_64		get;

	if (verif_corrupted_nm_64(ptr_header, endian, nm_data, &get) == 1)
		return (NULL);
	while ((get.i)++ < get.header_ncmds)
	{
		get.lc_cmd = (endian) ? endian_swap_int32(get.lc->cmd) : get.lc->cmd;
		if (get.lc_cmd == LC_SYMTAB)
		{
			get.sym_command = (struct symtab_command *)get.lc;
			get.nm_64->sym_list = get_sym_list_64(ptr_header, get.sym_command, \
				endian, nm_data);
			get.nm_64->sym_list_size = (endian) \
				? endian_swap_int32(get.sym_command->nsyms) \
				: get.sym_command->nsyms;
		}
		if (get.lc_cmd == LC_SEGMENT_64)
			get.sect_last_list = add_sect_64_to_list(get.nm_64, \
				(struct segment_command_64 *)get.lc, &(get.count_sect), \
				get.sect_last_list, endian, nm_data, ptr_header);
		get.lc = (endian) ? (void *)get.lc + endian_swap_int32(get.lc->cmdsize)\
			: (void *)get.lc + get.lc->cmdsize;
	}
	return (get.nm_64);
}
