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


void			hexdump_ppc_64(void *ptr_header, struct section_64 *sect, int8_t endian)
{
	uint64_t					addr;
	uint64_t					i;
	uint64_t					size;
	void						*content;

	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	content  = ptr_header + ((endian) ? endian_swap_int32(sect->offset) : sect->offset);
	i = 0;
	while (i < size)
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
		i++;
	}
	ft_putchar('\n');
}

void			hexdump_with_ascii_64(void *ptr_header, struct section_64 *sect, int8_t endian)
{
	uint64_t					addr;
	uint64_t					i;
	uint64_t					size;
	void						*content;

	if (is_ppc_arch_64(ptr_header, endian))
	{
		hexdump_ppc_64(ptr_header, sect, endian);
		return ;
	}
	addr = (endian) ? endian_swap_int64(sect->addr) : sect->addr;
	size = (endian) ? endian_swap_int64(sect->size) : sect->size;
	content  = ptr_header + ((endian) ? endian_swap_int32(sect->offset) : sect->offset);
	i = 0;
	while (i < size)
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
		i++;
	}
	ft_putchar('\n');
}

int8_t			identify_segment_64(void *ptr_header, struct segment_command_64 *seg, int8_t endian, \
	int8_t flag[2])
{
	struct section_64	*sect;
	size_t				i;
	uint32_t			seg_nsects;

	i = 0;
	sect = (void *)seg + sizeof(*seg);
	seg_nsects = (endian) ? endian_swap_int32(seg->nsects) : seg->nsects;
	while(i < seg_nsects)
	{
		if (flag[0] == 1 && ft_strcmp(sect[i].sectname, SECT_DATA) == 0 \
			&& ft_strcmp(sect[i].segname, SEG_DATA) == 0)
		{
			ft_printf("Contents of (__DATA,__data) section\n");
			hexdump_with_ascii_64(ptr_header, &(sect[i]), endian);
		}
		if (flag[1] == 1 && ft_strcmp(sect[i].sectname, SECT_TEXT) == 0 \
			&& ft_strcmp(sect[i].segname, SEG_TEXT) == 0)
		{
			ft_printf("Contents of (__TEXT,__text) section\n");
			hexdump_with_ascii_64(ptr_header, &(sect[i]), endian);
		}
		i++;
	}
	return (0);
}

int8_t			find_segments_and_sections_64(void *ptr_header, int8_t endian, int8_t flag[2])
{
	size_t						i;
	struct mach_header_64		*header;
	struct load_command			*lc;
	uint32_t					header_ncmds;
	uint32_t					lc_cmd;

	header = (struct mach_header_64 *)ptr_header;
	header_ncmds = (endian) ? endian_swap_int32(header->ncmds) : header->ncmds;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	i = 0;
	while (i++ < header_ncmds)
	{
		lc_cmd = (endian) ? endian_swap_int32(lc->cmd) : lc->cmd;
		if (lc_cmd == LC_SEGMENT_64)
		{
			identify_segment_64(ptr_header, (struct segment_command_64 *)lc, \
				endian, flag);
		}
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) : (void *)lc + lc->cmdsize;
	}
	return (0);
}
