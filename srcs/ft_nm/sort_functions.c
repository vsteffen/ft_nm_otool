/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_functions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:01:40 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 22:01:41 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t		sort_alphabetically_32(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t endian)
{
	int		cmp;

	cmp = ft_strcmp(elem1->sym_table_string, elem2->sym_table_string);
	if (cmp >= 0)
	{
		if (cmp == 0)
		{
			if (endian)
			{
				if (endian_swap_int32(elem1->elem.n_value) < \
					endian_swap_int32(elem2->elem.n_value))
					return (1);
			}
			else
			{
				if (elem1->elem.n_value < elem2->elem.n_value)
					return (1);
			}
		}
		return (0);
	}
	return (1);
}

int8_t		sort_reverse_32(struct s_sym_32 *elem1, struct s_sym_32 *elem2, \
	int8_t endian)
{
	int		cmp;

	cmp = ft_strcmp(elem1->sym_table_string, elem2->sym_table_string);
	if (cmp < 0)
	{
		if (cmp == 0)
		{
			if (endian)
			{
				if (endian_swap_int32(elem1->elem.n_value) > \
					endian_swap_int32(elem2->elem.n_value))
					return (1);
			}
			else
			{
				if (elem1->elem.n_value > elem2->elem.n_value)
					return (1);
			}
		}
		return (0);
	}
	return (1);
}

int8_t		sort_alphabetically_64(struct s_sym_64 *elem1, \
	struct s_sym_64 *elem2, int8_t endian)
{
	int		cmp;

	cmp = ft_strcmp(elem1->sym_table_string, elem2->sym_table_string);
	if (cmp >= 0)
	{
		if (cmp == 0)
		{
			if (endian)
			{
				if (endian_swap_int64(elem1->elem.n_value) < \
					endian_swap_int64(elem2->elem.n_value))
					return (1);
			}
			else
			{
				if (elem1->elem.n_value < elem2->elem.n_value)
					return (1);
			}
		}
		return (0);
	}
	return (1);
}

int8_t		sort_reverse_64(struct s_sym_64 *elem1, struct s_sym_64 *elem2, \
	int8_t endian)
{
	int		cmp;

	cmp = ft_strcmp(elem1->sym_table_string, elem2->sym_table_string);
	if (cmp < 0)
	{
		if (cmp == 0)
		{
			if (endian)
			{
				if (endian_swap_int64(elem1->elem.n_value) > \
					endian_swap_int64(elem2->elem.n_value))
					return (1);
			}
			else
			{
				if (elem1->elem.n_value > elem2->elem.n_value)
					return (1);
			}
		}
		return (0);
	}
	return (1);
}
