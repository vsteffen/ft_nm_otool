/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_nm_32_1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 19:55:19 by vsteffen          #+#    #+#             */
/*   Updated: 2018/09/06 19:55:35 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

void			init_get_nm_32(struct s_get_nm_32 *get, void *ptr_header, \
	int8_t endian)
{
	get->header = (struct mach_header *)ptr_header;
	get->header_ncmds = (endian) ? endian_swap_int32(get->header->ncmds) \
		: get->header->ncmds;
	get->lc = (struct load_command *)(ptr_header + sizeof(*(get->header)));
	get->count_sect = 1;
	get->i = 0;
	get->sect_last_list = NULL;
	get->nm_32 = (struct s_nm_32*)malloc(sizeof(struct s_nm_32));
	get->nm_32->sym_list = NULL;
	get->nm_32->sect_list = NULL;
}

int8_t			verif_corrupted_nm_32(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data, struct s_get_nm_32 *get)
{
	if (sizeof(*(get->header)) + sizeof(*(get->lc)) >= nm_data->file_size)
	{
		ft_printf("ft_nm: %s: corrupted binary.\n", nm_data->file_path);
		return (1);
	}
	init_get_nm_32(get, ptr_header, endian);
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

void			init_sect_list_32(struct s_add_sect_32 *add_sect, \
	struct s_get_nm_32 *get, int8_t endian, void *ptr_header)
{
	add_sect->seg = (struct segment_command *)get->lc;
	add_sect->sect_last_list = get->sect_last_list;
	add_sect->ptr_header = ptr_header;
	add_sect->endian = endian;
}

void			get_symtab_32(struct s_get_nm_32 *get, void *ptr_header, \
	int8_t endian, struct s_nm_data *nm_data)
{
	get->sym_command = (struct symtab_command *)get->lc;
	get->nm_32->sym_list = get_sym_list_32(ptr_header, get->sym_command, \
		endian, nm_data);
	get->nm_32->sym_list_size = (endian) \
		? endian_swap_int32(get->sym_command->nsyms) \
		: get->sym_command->nsyms;
}

struct s_nm_32	*get_nm_32(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_get_nm_32		get;
	struct s_add_sect_32	add_sect;

	if (verif_corrupted_nm_32(ptr_header, endian, nm_data, &get) == 1)
		return (NULL);
	while ((get.i)++ < get.header_ncmds)
	{
		get.lc_cmd = (endian) ? endian_swap_int32(get.lc->cmd) : get.lc->cmd;
		if (get.lc_cmd == LC_SYMTAB)
			get_symtab_32(&get, ptr_header, endian, nm_data);
		if (get.lc_cmd == LC_SEGMENT)
		{
			init_sect_list_32(&add_sect, &get, endian, ptr_header);
			get.sect_last_list = add_sect_32_to_list(get.nm_32, \
				&add_sect, &(get.count_sect), nm_data);
		}
		get.lc = (endian) ? (void *)get.lc + endian_swap_int32(get.lc->cmdsize)\
			: (void *)get.lc + get.lc->cmdsize;
	}
	return (get.nm_32);
}
