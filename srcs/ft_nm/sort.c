/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 16:37:54 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/11 16:37:56 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t				sort_alphabetically(const char *str1, const char *str2)
{
	int 	cmp;

	cmp = ft_strcmp(str1, str2);
	if (cmp >= 0)
		return (0);
	return (1);
}

struct s_sym_64		*sorted_merge(struct s_sym_64* elem1, \
	struct s_sym_64* elem2, int8_t (*sort_selected)(const char *, const char *))
{
	struct s_sym_64		*result;

	result = NULL;
	if (elem1 == NULL)
	    return (elem2);
	else if (elem2 == NULL)
	    return (elem1);
	if ((*sort_selected)(elem1->sym_table_string, elem2->sym_table_string))
	{
	    result = elem1;
	    result->next = sorted_merge(elem1->next, elem2, sort_selected);
	}
	else
	{
	    result = elem2;
	    result->next = sorted_merge(elem1, elem2->next, sort_selected);
	}
	return(result);
}

void				front_back_split(struct s_sym_64 *source, \
	struct s_sym_64 **front_ref, struct s_sym_64 **back_ref)
{
    struct s_sym_64		*fast;
    struct s_sym_64		*slow;

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

void merge_sort_64(struct s_sym_64 **head_ref, int8_t (*sort_selected)(const char *, const char *))
{
	struct s_sym_64		*head;
	struct s_sym_64		*elem1;
	struct s_sym_64		*elem2;

	head = *head_ref;
	if ((head == NULL) || (head->next == NULL))
	    return ;
	front_back_split(head, &elem1, &elem2);
	merge_sort_64(&elem1, sort_selected);
	merge_sort_64(&elem2, sort_selected);
	*head_ref = sorted_merge(elem1, elem2, sort_selected);
}

void		sort_nm_64(struct s_nm_64 *nm_64, int8_t sort_selected)
{
	if (sort_selected == 0)
		merge_sort_64(&(nm_64->sym_list), &sort_alphabetically);
}
