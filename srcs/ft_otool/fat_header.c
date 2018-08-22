/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fat_header.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 23:31:17 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 23:31:23 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

char		*get_architecture_name(cpu_type_t cputype)
{
	if (cputype == CPU_TYPE_VAX)
		return ("VAX");
	if (cputype == CPU_TYPE_X86 || cputype == CPU_TYPE_I386)
		return ("i386");
	if (cputype == CPU_TYPE_X86_64)
		return ("x86_64");
	if (cputype == CPU_TYPE_MC98000)
		return ("MC98000");
	if (cputype == CPU_TYPE_HPPA)
		return ("HPPA");
	if (cputype == CPU_TYPE_ARM)
		return ("ARM");
	if (cputype == CPU_TYPE_ARM64)
		return ("ARM64");
	if (cputype == CPU_TYPE_MC88000)
		return ("MC88000");
	if (cputype == CPU_TYPE_SPARC)
		return ("SPARC");
	if (cputype == CPU_TYPE_I860)
		return ("i860");
	if (cputype == CPU_TYPE_POWERPC)
		return ("ppc");
	if (cputype == CPU_TYPE_POWERPC64)
		return ("ppc64");
	return ("Unknown");
}

int8_t		set_fat_arch_32_nb_arch_and_i(struct s_fat *fat, void *ptr_header, \
	struct s_nm_data *nm_data, int8_t endian)
{
	if (sizeof(*(fat->fat_header)) + sizeof(*(fat->fat_arch_32)) >= \
		nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	fat->fat_header = (struct fat_header*)ptr_header;
	fat->fat_arch_32 = (struct fat_arch*)\
		(ptr_header + sizeof(struct fat_header));
	fat->nb_arch = (endian) ? endian_swap_int32(fat->fat_header->nfat_arch) \
		: fat->fat_header->nfat_arch;
	fat->i = 0;
	return (0);
}

int8_t		handle_fat_header_32(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_fat	fat;

	if (set_fat_arch_32_nb_arch_and_i(&fat, ptr_header, nm_data, endian) > 0)
		return (1);
	while (fat.i < fat.nb_arch)
	{
		if (sizeof(*(fat.fat_header)) + sizeof(*(fat.fat_arch_32)) * fat.i + \
			sizeof(*(fat.fat_arch_32)) >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat.fat_arch_32 = (struct fat_arch*)(ptr_header + \
				sizeof(struct fat_header) + sizeof(struct fat_arch) * fat.i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, \
			get_architecture_name((endian) \
			? endian_swap_int32(fat.fat_arch_32->cputype) \
			: fat.fat_arch_32->cputype));
		fat.offset_arch = (endian) ? endian_swap_int32(fat.fat_arch_32->offset)\
			: fat.fat_arch_32->offset;
		if (fat.offset_arch >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat.magic_number = *(uint32_t *)(ptr_header + fat.offset_arch);
		match_and_use_magic_number(ptr_header + fat.offset_arch, \
			fat.magic_number, nm_data);
		fat.i++;
	}
	return (0);
}

int8_t		set_fat_arch_64_nb_arch_and_i(struct s_fat *fat, void *ptr_header, \
	struct s_nm_data *nm_data, int8_t endian)
{
	if (sizeof(*(fat->fat_header)) + sizeof(*(fat->fat_arch_64)) >= \
		nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	fat->fat_header = (struct fat_header*)ptr_header;
	fat->fat_arch_64 = (struct fat_arch_64*)\
		(ptr_header + sizeof(struct fat_header));
	fat->nb_arch = (endian) ? endian_swap_int32(fat->fat_header->nfat_arch) \
		: fat->fat_header->nfat_arch;
	fat->i = 0;
	return (0);
}

int8_t		handle_fat_header_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct s_fat	fat;

	if (set_fat_arch_32_nb_arch_and_i(&fat, ptr_header, nm_data, endian) > 0)
		return (1);
	while (fat.i < fat.nb_arch)
	{
		if (sizeof(*(fat.fat_header)) + sizeof(*(fat.fat_arch_64)) * fat.i + \
			sizeof(*(fat.fat_arch_64)) >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat.fat_arch_64 = (struct fat_arch_64*)(ptr_header + \
			sizeof(struct fat_header) + sizeof(struct fat_arch) * fat.i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, \
			get_architecture_name((endian) \
			? endian_swap_int32(fat.fat_arch_64->cputype) \
			: fat.fat_arch_64->cputype));
		fat.offset_arch = (endian) ? endian_swap_int64(fat.fat_arch_64->offset)\
			: fat.fat_arch_64->offset;
		if (fat.offset_arch >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat.magic_number = *(uint32_t *)(ptr_header + fat.offset_arch);
		match_and_use_magic_number(ptr_header + fat.offset_arch, \
			fat.magic_number, nm_data);
		fat.i++;
	}
	return (0);
}
