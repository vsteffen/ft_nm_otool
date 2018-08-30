/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sections_64.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/30 17:27:24 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/30 17:27:26 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

void			hexdump_with_ascii_64_print(void *content, size_t i, \
	uint64_t addr)
{
	if (i % 16 == 0)
	{
		if (i > 0)
			ft_putchar('\n');
		ft_printf("%016llx\t%02x ", addr + i, *(uint8_t*)(content + i));
	}
	else
	{
		ft_printf("%02x ", *(uint8_t*)(content + i));
	}
}

int8_t			hexdump_with_ascii_64(void *ptr_header, \
	struct section_64 *sect, int8_t endian, struct s_nm_data *nm_data)
{
	uint64_t					addr;
	uint64_t					i;
	uint64_t					size;
	uint32_t					offset;
	void						*content;

	if (is_ppc_arch_64(ptr_header, endian))
		return (hexdump_ppc_64(ptr_header, sect, endian, nm_data));
	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	offset = (endian) ? endian_swap_int32(sect->offset) : sect->offset;
	if (offset + size > nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	content = ptr_header + offset;
	i = 0;
	while (i < size)
	{
		hexdump_with_ascii_64_print(content, i, addr);
		i++;
	}
	ft_putchar('\n');
	return (0);
}

int8_t			identify_segment_64(void *ptr_header, struct section_64	*sect, \
	int8_t endian, struct s_nm_data *nm_data)
{
	if (nm_data->flag[0] == 1 && ft_strcmp(sect[nm_data->ti].sectname, \
		SECT_DATA) == 0 && ft_strcmp(sect[nm_data->ti].segname, SEG_DATA) == 0)
	{
		ft_printf("Contents of (__DATA,__data) section\n");
		hexdump_with_ascii_64(ptr_header, &(sect[nm_data->ti]), endian, \
			nm_data);
	}
	if (nm_data->flag[1] == 1 && ft_strcmp(sect[nm_data->ti].sectname, \
		SECT_TEXT) == 0 && ft_strcmp(sect[nm_data->ti].segname, SEG_TEXT) == 0)
	{
		ft_printf("Contents of (__TEXT,__text) section\n");
		hexdump_with_ascii_64(ptr_header, &(sect[nm_data->ti]), endian, \
			nm_data);
	}
	return (0);
}

int8_t			iterate_segment_64(void *ptr_header, \
	struct segment_command_64 *seg, int8_t endian, struct s_nm_data *nm_data)
{
	struct section_64	*sect;
	uint32_t			seg_nsects;

	if ((size_t)((void*)seg - (void*)ptr_header) + sizeof(*seg) \
		+ sizeof(*sect) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	sect = (void *)seg + sizeof(*seg);
	seg_nsects = (endian) ? endian_swap_int32(seg->nsects) : seg->nsects;
	if (sizeof(*sect) * seg_nsects \
		+ (size_t)((void*)seg - (void*)ptr_header) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	nm_data->ti = 0;
	while (nm_data->ti < seg_nsects)
	{
		identify_segment_64(ptr_header, sect, endian, nm_data);
		nm_data->ti++;
	}
	return (0);
}

int8_t			find_segments_and_sections_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct segment_command_64	*seg;
	struct s_sect_tmp			tmp;

	if (sizeof(*header) + sizeof(*lc) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	header = (struct mach_header_64 *)ptr_header;
	tmp.header_ncmds = (endian) ? endian_swap_int32(header->ncmds) \
		: header->ncmds;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	if (sizeof(*lc) * ((tmp.header_ncmds == 0) ? 0 \
		: tmp.header_ncmds - 1) + sizeof(*seg) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	tmp.i = 0;
	while (tmp.i++ < tmp.header_ncmds)
	{
		tmp.lc_cmd = (endian) ? endian_swap_int32(lc->cmd) : lc->cmd;
		if (tmp.lc_cmd == LC_SEGMENT_64)
			iterate_segment_64(ptr_header, (struct segment_command_64 *)lc, \
				endian, nm_data);
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) \
			: (void *)lc + lc->cmdsize;
	}
	return (0);
}
