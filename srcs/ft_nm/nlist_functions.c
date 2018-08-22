/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nlist_functions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 00:24:29 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/23 00:24:30 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t					get_location(int8_t n_type)
{
	if ((n_type & N_STAB) > 0)
		return (0);
	if ((n_type & N_EXT) != N_EXT)
		return (1);
	return (2);
}

char					get_correct_letter(char letter, int8_t stab_ext_p_local)
{
	if (stab_ext_p_local == 1)
		return (letter + 32);
	return (letter);
}

struct s_sect_64		*get_section_64(struct s_sect_64 *sect_list, \
	uint32_t ordinal)
{
	while (sect_list)
	{
		if (sect_list->ordinal == ordinal)
			return (sect_list);
		sect_list = sect_list->next;
	}
	return (NULL);
}

struct s_sect_32		*get_section_32(struct s_sect_32 *sect_list, \
	uint32_t ordinal)
{
	while (sect_list)
	{
		if (sect_list->ordinal == ordinal)
			return (sect_list);
		sect_list = sect_list->next;
	}
	return (NULL);
}
