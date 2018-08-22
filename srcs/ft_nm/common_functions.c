/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:25:21 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/23 00:23:04 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int32_t		endian_swap_int32(uint32_t x)
{
	x = (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | \
		(x << 24);
	return (x);
}

int64_t		endian_swap_int64(uint64_t x)
{
	x = (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | \
		(x << 24);
	return (x);
}

void		free_nm_32(struct s_nm_32 *nm_32)
{
	struct s_sym_32		*sym_32_tmp;
	struct s_sect_32	*sect_32_tmp;

	if (nm_32 && nm_32->sym_list)
	{
		while (nm_32->sym_list)
		{
			sym_32_tmp = nm_32->sym_list;
			nm_32->sym_list = nm_32->sym_list->next;
			free(sym_32_tmp);
		}
		while (nm_32->sect_list)
		{
			sect_32_tmp = nm_32->sect_list;
			nm_32->sect_list = nm_32->sect_list->next;
			free(sect_32_tmp);
		}
	}
	if (nm_32)
		free(nm_32);
}

void		free_nm_64(struct s_nm_64 *nm_64)
{
	struct s_sym_64		*sym_64_tmp;
	struct s_sect_64	*sect_64_tmp;

	if (nm_64 && nm_64->sym_list)
	{
		while (nm_64->sym_list)
		{
			sym_64_tmp = nm_64->sym_list;
			nm_64->sym_list = nm_64->sym_list->next;
			free(sym_64_tmp);
		}
		while (nm_64->sect_list)
		{
			sect_64_tmp = nm_64->sect_list;
			nm_64->sect_list = nm_64->sect_list->next;
			free(sect_64_tmp);
		}
	}
	if (nm_64)
		free(nm_64);
}
