/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   symbole_table_64.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/30 18:46:24 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/30 18:46:26 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t		iter_sym_table_and_print_sect_64(struct s_nm_64 *nm_64, \
	struct s_sym_64 *sym, int8_t stab_ext_p_local)
{
	struct s_sect_64	*sect;

	if (!(sect = get_section_64(nm_64->sect_list, sym->elem.n_sect)))
	{
		ft_putstr("\nft_nm: Unknown section\n");
		return (-1);
	}
	if (ft_strcmp(sect->elem.sectname, SECT_TEXT) == 0)
		ft_printf(" %c ", get_correct_letter('T', stab_ext_p_local));
	else if (ft_strcmp(sect->elem.sectname, SECT_DATA) == 0)
		ft_printf(" %c ", get_correct_letter('D', stab_ext_p_local));
	else if (ft_strcmp(sect->elem.sectname, SECT_BSS) == 0)
		ft_printf(" %c ", get_correct_letter('B', stab_ext_p_local));
	else
		ft_printf(" %c ", get_correct_letter('S', stab_ext_p_local));
	return (1);
}

int8_t		iter_sym_table_and_print_value_64(struct s_nm_64 *nm_64, \
	struct s_sym_64 *sym, int8_t stab_ext_p_local, int8_t real_type)
{
	if ((real_type & N_SECT) == N_SECT)
	{
		if (iter_sym_table_and_print_sect_64(nm_64, sym, stab_ext_p_local) \
			== -1)
			return (-1);
	}
	else if ((real_type & N_ABS) == N_ABS)
		ft_printf(" %c ", get_correct_letter('A', stab_ext_p_local));
	else if ((real_type & N_PBUD) == N_PBUD)
		ft_printf(" u ");
	else if ((real_type & N_INDR) == N_INDR)
		ft_printf(" %c ", get_correct_letter('I', stab_ext_p_local));
	else if ((real_type & N_UNDF) == N_UNDF)
		ft_printf(" %c ", get_correct_letter('U', stab_ext_p_local));
	else
	{
		ft_putstr("ft_nm: Unknown symbol table type\n");
		return (-1);
	}
	return (1);
}

int8_t		iter_sym_table_and_print_64(struct s_nm_64 *nm_64, int8_t endian)
{
	struct s_sym_64		*sym;
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
				ft_printf("%016llx", (endian) \
					? endian_swap_int64(sym->elem.n_value) : sym->elem.n_value);
			else
				ft_putstr("                ");
			if (iter_sym_table_and_print_value_64(nm_64, sym, stab_ext_p_local,\
				real_type) == -1)
				return (-1);
			ft_printf("%s\n", sym->sym_table_string);
		}
		sym = sym->next;
	}
	return (0);
}
