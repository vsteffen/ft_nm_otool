/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_32.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:00:52 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 22:00:53 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

static struct s_sym_32	*sorted_merge(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t (*sort_selected)(struct s_sym_32*, \
	struct s_sym_32*, int8_t endian), int8_t endian)
{
	struct s_sym_32		*result;

	result = NULL;
	if (elem1 == NULL)
		return (elem2);
	else if (elem2 == NULL)
		return (elem1);
	if ((*sort_selected)(elem1, elem2, endian))
	{
		result = elem1;
		result->next = sorted_merge(elem1->next, elem2, sort_selected, endian);
	}
	else
	{
		result = elem2;
		result->next = sorted_merge(elem1, elem2->next, sort_selected, endian);
	}
	return (result);
}

static void				front_back_split(struct s_sym_32 *source, \
	struct s_sym_32 **front_ref, struct s_sym_32 **back_ref)
{
	struct s_sym_32		*fast;
	struct s_sym_32		*slow;

	slow = source;
	fast = source->next;
	while (fast != NULL)
	{
		fast = fast->next;
		if (fast != NULL)
		{
			slow = slow->next;
			fast = fast->next;
		}
	}
	*front_ref = source;
	*back_ref = slow->next;
	slow->next = NULL;
}

void					merge_sort_32(struct s_sym_32 **head_ref, \
	int8_t (*sort_selected)(struct s_sym_32*, struct s_sym_32*, int8_t endian),\
	int8_t endian)
{
	struct s_sym_32		*head;
	struct s_sym_32		*elem1;
	struct s_sym_32		*elem2;

	head = *head_ref;
	if ((head == NULL) || (head->next == NULL))
		return ;
	front_back_split(head, &elem1, &elem2);
	merge_sort_32(&elem1, sort_selected, endian);
	merge_sort_32(&elem2, sort_selected, endian);
	*head_ref = sorted_merge(elem1, elem2, sort_selected, endian);
}

void					sort_nm_32(struct s_nm_32 *nm_32, int8_t endian, \
	struct s_nm_data *nm_data)
{
	if (nm_data->flag[0] == 1)
		return ;
	if (nm_data->flag[1] == 1)
		merge_sort_32(&(nm_32->sym_list), &sort_reverse_32, endian);
	else
		merge_sort_32(&(nm_32->sym_list), &sort_alphabetically_32, endian);
}
