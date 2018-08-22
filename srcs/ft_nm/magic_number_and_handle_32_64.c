/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   magic_number_and_handle_32_64.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 00:02:49 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/23 00:02:50 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t		handle_64_nm(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_nm_64				*nm_64;
	int8_t						res;

	nm_64 = get_nm_64(ptr_header, endian, nm_data);
	sort_nm_64(nm_64, endian, nm_data);
	if (nm_64->sym_list)
		res = iter_sym_table_and_print_64(nm_64, endian);
	else
	{
		ft_printf("ft_nm: %s: The file was not recognized \
as a valid object file\n", nm_data->file_path);
		res = -1;
	}
	free_nm_64(nm_64);
	return (res);
}

int8_t		handle_32_nm(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_nm_32				*nm_32;
	int8_t						res;

	nm_32 = get_nm_32(ptr_header, endian, nm_data);
	sort_nm_32(nm_32, endian, nm_data);
	if (nm_32->sym_list)
		res = iter_sym_table_and_print_32(nm_32, endian);
	else
	{
		ft_printf("ft_nm: %s: The file was not recognized \
as a valid object file\n", nm_data->file_path);
		res = -1;
	}
	free_nm_32(nm_32);
	return (res);
}

int8_t		match_and_use_magic_number(void *ptr_header, uint32_t magic_number,\
	struct s_nm_data *nm_data)
{
	if (magic_number == MH_MAGIC_64)
		return (handle_64_nm(ptr_header, 0, nm_data));
	else if (magic_number == MH_CIGAM_64)
		return (handle_64_nm(ptr_header, 1, nm_data));
	else if (magic_number == MH_MAGIC)
		return (handle_32_nm(ptr_header, 0, nm_data));
	else if (magic_number == MH_CIGAM)
		return (handle_32_nm(ptr_header, 1, nm_data));
	else if (magic_number == AR_MAGIC)
		return (handle_ar(ptr_header, nm_data));
	ft_printf("ft_nm: %s: The file was not recognized \
as a valid object file\n", nm_data->file_path);
	return (-2);
}

int8_t		analyse_magic_number(void *ptr_header, struct s_nm_data *nm_data)
{
	uint32_t		magic_number;

	if (!ptr_header)
	{
		ft_printf("ft_nm: %s: The file was not recognized \
as a valid object file\n", nm_data->file_path);
		return (-1);
	}
	magic_number = *(uint32_t *)ptr_header;
	if (magic_number == FAT_MAGIC)
		return (handle_fat_header_32(ptr_header, 0, nm_data));
	else if (magic_number == FAT_CIGAM)
		return (handle_fat_header_32(ptr_header, 1, nm_data));
	else if (magic_number == FAT_MAGIC_64)
		return (handle_fat_header_64(ptr_header, 0, nm_data));
	else if (magic_number == FAT_CIGAM_64)
		return (handle_fat_header_64(ptr_header, 1, nm_data));
	return (match_and_use_magic_number(ptr_header, magic_number, nm_data));
}
