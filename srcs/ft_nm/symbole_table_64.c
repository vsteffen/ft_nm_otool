#include "ft_nm_otool.h"

// CHEAT SHEET

// debug -> mask N_STAB on n_type
// external -> mask N_EXT on n_type
// private external -> mask N_PEXT on n_type
// non external -> mask N_EXT and N_PEXT on n_type

// Get true type with mask N_TYPE on n_type
// undefined -> mask N_UNDF with N_TYPE
// absolute -> mask N_ABS with N_TYPE
// indirect -> mask N_INDR with N_TYPE
// section -> mask N_SECT with N_TYPE
// common -> masks N_UNDF and N_EXT with n_value > 0

int8_t		iter_sym_table_and_print_64(struct s_nm_64 *nm_64)
{
	struct s_sym_64		*sym;
	struct s_sect_64	*sect;
	int8_t				stab_ext_p_local;
	int8_t				real_type;

	sym = nm_64->sym_list;
	while (sym)
	{
		stab_ext_p_local = get_location(sym->elem.n_type);
		real_type = (sym->elem.n_type & N_TYPE);
		if (stab_ext_p_local > 0)
		{
			if (real_type != N_UNDF)
				ft_printf("%016llx", sym->elem.n_value);
			else
				ft_putstr("                ");
			// if (stab_ext_p_local == 2 && (real_type & N_UNDF) == N_UNDF && sym->elem.n_value > 0) // Detect common
			// 	ft_printf(" %c ", get_correct_letter('C', stab_ext_p_local));
			if ((real_type & N_SECT) == N_SECT) // section
			{
				if (!(sect = get_section_64(nm_64->sect_list, sym->elem.n_sect)))
				{
					ft_putstr("\nft_nm: Unknown section\n");
					return (-1);
				}

				// if (sec != NULL)
					// ft_printf(" (%s,%s) ", sect->elem.segname, sect->elem.sectname);

				if (ft_strcmp(sect->elem.sectname, SECT_TEXT) == 0)
					ft_printf(" %c ", get_correct_letter('T', stab_ext_p_local));
				else if (ft_strcmp(sect->elem.sectname, SECT_DATA) == 0)
					ft_printf(" %c ", get_correct_letter('D', stab_ext_p_local));
				else if (ft_strcmp(sect->elem.sectname, SECT_BSS) == 0)
					ft_printf(" %c ", get_correct_letter('B', stab_ext_p_local));
				else
					ft_printf(" %c ", get_correct_letter('S', stab_ext_p_local));
			}
			else if ((real_type & N_UNDF) == N_UNDF) // detect undefined
			{
					ft_printf(" %c ", get_correct_letter('U', stab_ext_p_local));
			}
			else if ((real_type & N_ABS) == N_ABS) // detect absolute
				ft_printf(" %c ", get_correct_letter('A', stab_ext_p_local));
			else if ((real_type & N_PBUD) == N_PBUD)
					ft_printf(" u "); //, get_correct_letter('U', stab_ext_p_local));
			else if ((real_type & N_INDR) == N_INDR) // detect indirect
				ft_printf(" %c ", get_correct_letter('I', stab_ext_p_local));
			else
			{
				ft_putstr("ft_nm: Unknown symbol table type\n");
				return (-1);
			}
			ft_printf("%s\n", sym->sym_table_string);
		}
		sym = sym->next;
	}
	return (0);
}
