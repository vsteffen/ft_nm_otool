/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ar_2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:32:07 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 22:32:10 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

void		find_next_ar_header(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data)
{
	char		*file;

	file = (char *)ptr_header;
	while (*i < nm_data->file_size)
	{
		if (ft_strncmp(ARFMAG, file + *i, ft_strlen(ARFMAG)) == 0)
		{
			*i += 2;
			return ;
		}
		(*i)++;
	}
}

void		*find_begin_ar_file(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data)
{
	char		*file;

	file = (char *)ptr_header;
	while (file[*i] != '\0')
		(*i)++;
	*i += sizeof(uint32_t) - (*i % sizeof(uint32_t));
	while (*i < nm_data->file_size)
	{
		if (*(uint32_t*)(ptr_header + *i) > 0)
			return (ptr_header + *i);
		*i += sizeof(uint32_t);
	}
	return (NULL);
}
