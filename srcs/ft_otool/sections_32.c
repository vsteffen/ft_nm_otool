#include "ft_nm_otool.h"

int8_t			is_ppc_arch_32(void *ptr_header, int8_t endian)
{
	struct mach_header		*header;
	cpu_type_t				cputype;

	header = (struct mach_header *)ptr_header;
	cputype = (endian) ? endian_swap_int32(header->cputype) : header->cputype;
	if (cputype == CPU_TYPE_POWERPC || cputype == CPU_TYPE_POWERPC64)
		return (1);
	return (0);
}


int8_t			hexdump_ppc_32(void *ptr_header, struct section *sect, int8_t endian, struct s_nm_data *nm_data)
{
	uint32_t				addr;
	uint32_t				i;
	uint32_t				size;
	uint32_t				offset;
	void					*content;

	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	offset = (endian) ? endian_swap_int32(sect->offset) : sect->offset;
	if (offset + size > nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	content  = ptr_header + offset;
	i = 0;
	while (i < size)
	{
		if (i % 16 == 0)
		{
			if (i > 0)
				ft_putchar('\n');
			ft_printf("%08llx\t%02x", addr + i, *(uint8_t*)(content + i));
		}
		else
		{
			if (i % 4 == 0)
				ft_putchar(' ');
			ft_printf("%02x", *(uint8_t*)(content + i));
		}
		i++;
	}
	ft_putchar('\n');
	return (0);
}

int8_t			hexdump_with_ascii_32(void *ptr_header, struct section *sect, int8_t endian, struct s_nm_data *nm_data)
{
	uint32_t		addr;
	uint32_t		i;
	uint32_t		size;
	uint32_t		offset;
	void			*content;

	if (is_ppc_arch_32(ptr_header, endian))
		return (hexdump_ppc_32(ptr_header, sect, endian, nm_data));
	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	offset = (endian) ? endian_swap_int32(sect->offset) : sect->offset;
	if (offset + size > nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	content = ptr_header + offset;
	i = 0;
	while (i < size)
	{
		if (i % 16 == 0)
		{
			if (i > 0)
				ft_putchar('\n');
			ft_printf("%08llx\t%02x ", addr + i, *(uint8_t*)(content + i));
		}
		else
		{
			ft_printf("%02x ", *(uint8_t*)(content + i));
		}
		i++;
	}
	ft_putchar('\n');
	return (0);
}

int8_t			identify_segment_32(void *ptr_header, struct segment_command *seg, int8_t endian, \
	struct s_nm_data *nm_data)
{
	struct section		*sect;
	size_t				i;
	uint32_t			seg_nsects;

	if ((size_t)((void*)seg - (void*)ptr_header) + sizeof(*seg) + sizeof(*sect) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	sect = (void *)seg + sizeof(*seg);
	seg_nsects = (endian) ? endian_swap_int32(seg->nsects) : seg->nsects;
	if (sizeof(*sect) * seg_nsects + (size_t)((void*)seg - (void*)ptr_header) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	i = 0;
	while(i < seg_nsects)
	{
		if (nm_data->flag[0] == 1 && ft_strcmp(sect[i].sectname, SECT_DATA) == 0 \
			&& ft_strcmp(sect[i].segname, SEG_DATA) == 0)
		{
			ft_printf("Contents of (__DATA,__data) section\n");
			hexdump_with_ascii_32(ptr_header, &(sect[i]), endian, nm_data);
		}
		if (nm_data->flag[1] == 1 && ft_strcmp(sect[i].sectname, SECT_TEXT) == 0 \
			&& ft_strcmp(sect[i].segname, SEG_TEXT) == 0)
		{
			ft_printf("Contents of (__TEXT,__text) section\n");
			hexdump_with_ascii_32(ptr_header, &(sect[i]), endian, nm_data);
		}
		i++;
	}
	return (0);
}

int8_t			find_segments_and_sections_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	size_t						i;
	struct mach_header			*header;
	struct load_command			*lc;
	struct segment_command		*seg;
	uint32_t					header_ncmds;
	uint32_t					lc_cmd;

	if (sizeof(*header) + sizeof(*lc) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	header = (struct mach_header *)ptr_header;
	header_ncmds = (endian) ? endian_swap_int32(header->ncmds) : header->ncmds;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	if (sizeof(*lc) * ((header_ncmds == 0) ? 0 : header_ncmds - 1) + sizeof(*seg) >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	i = 0;
	while (i++ < header_ncmds)
	{
		lc_cmd = (endian) ? endian_swap_int32(lc->cmd) : lc->cmd;
		if (lc_cmd == LC_SEGMENT)
		{
			identify_segment_32(ptr_header, (struct segment_command *)lc, endian, nm_data);
		}
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) : (void *)lc + lc->cmdsize;
	}
	return (0);
}
