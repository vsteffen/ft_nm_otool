/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ar_1.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:28:41 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 22:28:43 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

uint64_t	get_ran_off_64(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data)
{
	*i += ft_strlen((char*)(ptr_header + *i));
	*i += sizeof(uint64_t) - (*i % sizeof(uint64_t));
	while (*i < nm_data->file_size)
	{
		if (*(uint64_t*)(ptr_header + *i) > 0)
		{
			*i += sizeof(uint64_t);
			return (*(uint64_t*)(ptr_header + *i) - sizeof(uint64_t));
		}
		*i += sizeof(uint64_t);
	}
	return (0);
}

uint32_t	get_ran_off_32(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data)
{
	*i += ft_strlen((char*)(ptr_header + *i));
	*i += sizeof(uint32_t) - (*i % sizeof(uint32_t));
	while (*i < nm_data->file_size)
	{
		if (*(uint32_t*)(ptr_header + *i) > 0)
		{
			*i += sizeof(uint32_t);
			return (*(uint32_t*)(ptr_header + *i - sizeof(uint32_t)));
		}
		*i += sizeof(uint32_t);
	}
	return (0);
}

int8_t		set_ran_off(struct s_ar *ar, void *ptr_header, \
	struct s_nm_data *nm_data)
{
	if (ar->i >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	if (ft_strncmp(SYMDEF, ptr_header + ar->i, ft_strlen(SYMDEF)) == 0 || \
		ft_strncmp(SYMDEF_SORTED, ptr_header + ar->i, \
		ft_strlen(SYMDEF_SORTED)) == 0)
	{
		ar->is_32_ar = 1;
		ar->ran_off = get_ran_off_32(ptr_header, &(ar->i), nm_data);
	}
	else if (ft_strncmp(SYMDEF_64, ptr_header + ar->i, ft_strlen(SYMDEF_64)) \
		== 0 || ft_strncmp(SYMDEF_64_SORTED, ptr_header + ar->i, \
		ft_strlen(SYMDEF_64_SORTED)) == 0)
		ar->ran_off = get_ran_off_64(ptr_header, &(ar->i), nm_data);
	else
		return (1);
	return (0);
}

int8_t		ar_iter_files(struct s_ar *ar, void *ptr_header, \
	struct s_nm_data *nm_data)
{
	ar->i = (ar->is_32_ar) ? *(uint32_t*)(ptr_header + ar->j) : \
		*(uint64_t*)(ptr_header + ar->symtab_pos + ar->j);
	if (ar->i >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	find_next_ar_header(ptr_header, &(ar->i), nm_data);
	if (ar->i >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	if (ar->offset_file_tmp == (uint64_t)-1 || ar->i != ar->offset_file_tmp)
	{
		ar->offset_file_tmp = ar->i;
		ft_printf("\n%s(%s):\n", nm_data->file_path, ptr_header + ar->i);
		find_begin_ar_file(ptr_header, &(ar->i), nm_data);
		if (ar->i >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		match_and_use_magic_number(ptr_header + ar->i, \
			*(uint32_t *)(ptr_header + ar->i), nm_data);
	}
	ar->j += 2 * ((ar->is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	return (0);
}

int8_t		handle_ar(void *ptr_header, struct s_nm_data *nm_data)
{
	struct s_ar		ar;
	int8_t			res;

	ar.i = 0;
	ar.is_32_ar = 0;
	find_next_ar_header(ptr_header, &(ar.i), nm_data);
	if ((res = set_ran_off(&ar, ptr_header, nm_data)) > 0)
		return (res);
	ar.symtab_pos = ar.i;
	if (ar.ran_off == 0 || ar.ran_off + ar.symtab_pos >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	ar.j = ar.i + ((ar.is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	ar.offset_file_tmp = (uint64_t)-1;
	while (ar.j < ar.ran_off + ar.symtab_pos)
	{
		if ((res = ar_iter_files(&ar, ptr_header, nm_data)) > 0)
			return (res);
	}
	return (0);
}
