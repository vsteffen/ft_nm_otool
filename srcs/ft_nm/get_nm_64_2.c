/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_nm_64_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 19:54:57 by vsteffen          #+#    #+#             */
/*   Updated: 2018/09/06 19:54:58 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t				init_get_sym_64(struct s_get_sym_64 *get_sym, \
	void *ptr_header, struct symtab_command *sym_command, \
	struct s_nm_data *nm_data)
{
	get_sym->sym_elem_first = NULL;
	get_sym->sym_elem_prev = NULL;
	if (get_sym->endian)
	{
		get_sym->sym_table_entry = ptr_header \
			+ endian_swap_int32(sym_command->symoff);
		get_sym->sym_table_string = ptr_header \
			+ endian_swap_int32(sym_command->stroff);
		get_sym->sym_command_nsyms = endian_swap_int32(sym_command->nsyms);
	}
	else
	{
		get_sym->sym_table_entry = ptr_header + sym_command->symoff;
		get_sym->sym_table_string = ptr_header + sym_command->stroff;
		get_sym->sym_command_nsyms = sym_command->nsyms;
	}
	if ((size_t)((void*)get_sym->sym_table_entry - (void*)ptr_header) \
		+ sizeof(*sym_command) + sizeof(*(get_sym->sym_table_entry)) \
		* get_sym->sym_command_nsyms >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (0);
	}
	get_sym->i = 0;
	return (1);
}

int8_t				add_sym_elem_64(struct s_get_sym_64 *get_sym, \
	struct s_nm_data *nm_data, void *ptr_header, int8_t endian)
{
	get_sym->sym_elem = (struct s_sym_64 *)malloc(sizeof(struct s_sym_64));
	get_sym->sym_elem->elem = get_sym->sym_table_entry[get_sym->i];
	get_sym->sym_table_string_elem = (endian) ? get_sym->sym_table_string \
		+ endian_swap_int32(get_sym->sym_table_entry[get_sym->i].n_un.n_strx)\
		: get_sym->sym_table_string \
		+ get_sym->sym_table_entry[get_sym->i].n_un.n_strx;
	if ((size_t)((void*)get_sym->sym_table_string - (void*)ptr_header) \
		> nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (0);
	}
	get_sym->sym_elem->sym_table_string = get_sym->sym_table_string_elem;
	get_sym->sym_elem->order = get_sym->i;
	if (get_sym->i == 0)
		get_sym->sym_elem_first = get_sym->sym_elem;
	if (get_sym->sym_elem_prev)
		get_sym->sym_elem_prev->next = get_sym->sym_elem;
	get_sym->sym_elem_prev = get_sym->sym_elem;
	get_sym->i++;
	return (1);
}

struct s_sym_64		*get_sym_list_64(void *ptr_header, \
	struct symtab_command *sym_command, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_get_sym_64		get_sym;

	get_sym.endian = endian;
	if (init_get_sym_64(&get_sym, ptr_header, sym_command, nm_data) == 0)
		return (NULL);
	while (get_sym.i < get_sym.sym_command_nsyms)
	{
		if (add_sym_elem_64(&get_sym, nm_data, ptr_header, endian) == 0)
			return (NULL);
	}
	if (get_sym.i > 0)
		get_sym.sym_elem->next = NULL;
	return (get_sym.sym_elem_first);
}

void				add_sect_64_elem(struct s_add_sect_64 *add_sect, \
	struct s_nm_64 *nm_64, size_t *count_sect, struct s_nm_data *nm_data)
{
	add_sect->sect_elem = (struct s_sect_64 *)malloc(sizeof(struct s_sect_64));
	add_sect->sect_elem->elem = add_sect->sect[nm_data->ti];
	add_sect->sect_elem->ordinal = *count_sect;
	if (add_sect->sect_last_list)
		add_sect->sect_last_list->next = add_sect->sect_elem;
	add_sect->sect_last_list = add_sect->sect_elem;
	if (!nm_64->sect_list)
		nm_64->sect_list = add_sect->sect_elem;
	(*count_sect)++;
	nm_data->ti++;
}

struct s_sect_64	*add_sect_64_to_list(struct s_nm_64 *nm_64, \
	struct s_add_sect_64 *add_sect, size_t *count_sect, \
	struct s_nm_data *nm_data)
{
	uint32_t			seg_nsects;

	add_sect->sect = (void *)add_sect->seg + sizeof(*(add_sect->seg));
	add_sect->sect_elem = NULL;
	seg_nsects = (add_sect->endian) ? endian_swap_int32(add_sect->seg->nsects) \
		: add_sect->seg->nsects;
	if ((size_t)((void*)add_sect->sect - (void*)add_sect->ptr_header) \
		+ sizeof(*(add_sect->sect)) * seg_nsects >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (NULL);
	}
	nm_data->ti = 0;
	while (nm_data->ti < seg_nsects)
		add_sect_64_elem(add_sect, nm_64, count_sect, nm_data);
	if (add_sect->sect_elem)
		add_sect->sect_elem->next = NULL;
	if (nm_data->ti == 0)
		return (add_sect->sect_last_list);
	return (add_sect->sect_elem);
}
