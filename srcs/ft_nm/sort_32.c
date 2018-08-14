#include "ft_nm_otool.h"

static int8_t				sort_alphabetically(struct s_sym_32 *elem1, struct s_sym_32 *elem2, int8_t endian)
{
	int 	cmp;

	cmp = ft_strcmp(elem1->sym_table_string, elem2->sym_table_string);
	if (cmp >= 0)
	{
		if (cmp == 0)
		{
			if (endian)
			{
				if (endian_swap_int32(elem1->elem.n_value) < endian_swap_int32(elem2->elem.n_value))
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


static struct s_sym_32		*sorted_merge(struct s_sym_32* elem1, \
	struct s_sym_32* elem2, \
	int8_t (*sort_selected)(struct s_sym_32*, struct s_sym_32*, int8_t endian), \
	int8_t endian)
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
	return(result);
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

void merge_sort_32(struct s_sym_32 **head_ref, \
	int8_t (*sort_selected)(struct s_sym_32*, struct s_sym_32*, int8_t endian), \
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

void		sort_nm_32(struct s_nm_32 *nm_32, int8_t sort_selected, int8_t endian)
{
	if (sort_selected == 0)
		merge_sort_32(&(nm_32->sym_list), &sort_alphabetically, endian);
}
