/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections_64_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/30 18:39:00 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/30 18:39:01 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t			is_ppc_arch_64(void *ptr_header, int8_t endian)
{
	struct mach_header_64		*header;
	cpu_type_t					cputype;

	header = (struct mach_header_64 *)ptr_header;
	cputype = (endian) ? endian_swap_int32(header->cputype) : header->cputype;
	if (cputype == CPU_TYPE_POWERPC || cputype == CPU_TYPE_POWERPC64)
		return (1);
	return (0);
}

void			hexdump_ppc_64_print(void *content, size_t i, \
	uint64_t addr)
{
	if (i % 16 == 0)
	{
		if (i > 0)
			ft_putchar('\n');
		ft_printf("%016llx\t%02x", addr + i, *(uint8_t*)(content + i));
	}
	else
	{
		if (i % 4 == 0)
			ft_putchar(' ');
		ft_printf("%02x", *(uint8_t*)(content + i));
	}
}

int8_t			hexdump_ppc_64(void *ptr_header, struct section_64 *sect, \
	int8_t endian, struct s_nm_data *nm_data)
{
	uint64_t					addr;
	uint64_t					i;
	uint64_t					size;
	uint32_t					offset;
	void						*content;

	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	offset = (endian) ? endian_swap_int32(sect->offset) : sect->offset;
	if (offset + size > nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	content = ptr_header + offset;
	i = 0;
	while (i < size)
	{
		hexdump_ppc_64_print(content, i, addr);
		i++;
	}
	ft_putchar('\n');
	return (0);
}
