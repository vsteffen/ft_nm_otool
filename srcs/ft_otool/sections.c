#include "ft_nm_otool.h"

void			identify_segment_32(struct segment_command *seg, int8_t endian, \
	int8_t flag[2])
{
	struct section		*sect;
	size_t				i;
	uint32_t			seg_nsects;

	i = 0;
	sect = (void *)seg + sizeof(*seg);
	seg_nsects = (endian) ? endian_swap_int32(seg->nsects) : seg->nsects;
	while(i < seg_nsects)
	{
		if (flag[0] == 1 && ft_strcmp(sect->sectname, SECT_DATA) == 0 && ft_strcmp(sect->segname, SEG_DATA))
		{
			ft_printf("Contents of (__DATA,__data) section\n");
		}
		if (flag[1] == 1 && ft_strcmp(sect->sectname, SECT_TEXT) == 0 && ft_strcmp(sect->segname, SEG_TEXT))
		{
			ft_printf("Contents of (__TEXT,__text) section\n");
		}
		i++;
	}
}

int8_t			find_segments_and_sections_32(void *ptr_header, int8_t endian, int8_t flag[2])
{
	size_t						i;
	struct mach_header			*header;
	struct load_command			*lc;
	uint32_t					header_ncmds;
	uint32_t					lc_cmd;

	header = (struct mach_header *)ptr_header;
	header_ncmds = (endian) ? endian_swap_int32(header->ncmds) : header->ncmds;
	lc = (struct load_command *)(ptr_header + sizeof(*header));
	i = 0;
	while (i++ < header_ncmds)
	{
		lc_cmd = (endian) ? endian_swap_int32(lc->cmd) : lc->cmd;
		if (lc_cmd == LC_SEGMENT)
		{
			identify_segment_32((struct segment_command *)lc, endian, flag);
		}
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) : (void *)lc + lc->cmdsize;
	}
	return (0);
}

int8_t			identify_segment_64(struct segment_command_64 *seg, int8_t endian, \
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
		if (flag[0] == 1 && ft_strcmp(sect->sectname, SECT_DATA) == 0 && ft_strcmp(sect->segname, SEG_DATA))
		{
			ft_printf("Contents of (__DATA,__data) section\n");
		}
		if (flag[1] == 1 && ft_strcmp(sect->sectname, SECT_TEXT) == 0 && ft_strcmp(sect->segname, SEG_TEXT))
		{
			ft_printf("Contents of (__TEXT,__text) section\n");
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
		if (lc_cmd == LC_SEGMENT)
		{
			identify_segment_64((struct segment_command_64 *)lc, endian, flag);
		}
		lc = (endian) ? (void *)lc + endian_swap_int32(lc->cmdsize) : (void *)lc + lc->cmdsize;
	}
	return (0);
}
