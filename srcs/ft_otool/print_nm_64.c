#include "ft_nm_otool.h"

void		print_sections_64_deprecated(struct segment_command_64 *seg)
{
	size_t				i;
	struct section_64	*sect;
	i = 0;
	sect = (void *)seg + sizeof(*seg);
	while (i < seg->nsects)
	{
		ft_printf(" └─> Section [%s]\n", sect[i].sectname);
		i++;
	}
}

void		print_segments_64_deprecated(char *content)
{
	size_t						i;
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct segment_command_64	*seg;

	header = (struct mach_header_64 *)content;
	lc = (void *)content + sizeof(*header);
	i = 0;
	seg = NULL;
	while (i < header->ncmds)
	{
		if (lc->cmd == LC_SEGMENT_64)
		{
			seg = (struct segment_command_64 *)lc;
			ft_printf("\nSEGMENT [%s] have [%d] section(s)\n", seg->segname, seg->nsects);
			print_sections_64_deprecated(seg);
		}
		i++;
		lc = (void *)lc + lc->cmdsize;
	}
	ft_printf("\n");
}

void		print_sections_64(struct s_sect_64 *sect_list)
{
	while (sect_list)
	{
		ft_printf(" --> Section [%s] n˚[%d]\n", sect_list->elem.sectname, sect_list->ordinal);
		sect_list = sect_list->next;
	}
}

void		print_symboles_64(struct s_sym_64 *sym_list)
{
	while (sym_list)
	{
		ft_printf(" --> Symbole [%s]\n", sym_list->sym_table_string);
		sym_list = sym_list->next;
	}
}
