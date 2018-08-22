/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/22 22:59:03 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/22 22:59:05 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t		is_flag_otool(char *arg)
{
	size_t		i;

	if (arg[0] != '-')
		return (-1);
	i = 0;
	while (arg[++i] != '\0')
	{
		if (arg[i] == 'd')
			continue ;
		if (arg[i] == 't')
			continue ;
		return (-2);
	}
	return (1);
}

int8_t		get_flag_otool(int8_t flag[2], int ac, char **av)
{
	int			i;
	size_t		j;

	i = 1;
	while (i++ < ac)
	{
		j = 0;
		while (av[i - 1][++j] != '\0')
		{
			if (av[i - 1][0] != '-')
				break ;
			if (av[i - 1][j] == 'd')
			{
				flag[0] = 1;
				continue ;
			}
			if (av[i - 1][j] == 't')
			{
				flag[1] = 1;
				continue ;
			}
			return (0);
		}
	}
	return (1);
}

int8_t		verif_files_and_print_filename_if_one_file(int ac, char **av)
{
	int			i;
	int			files;
	int			file_ac;
	int			flags;
	int			res;

	files = 0;
	flags = 0;
	i = 1;
	while (i++ < ac)
	{
		if ((res = is_flag_otool(av[i - 1])) == -1)
		{
			file_ac = i - 1;
			files++;
		}
		if (res > 0)
			flags++;
	}
	if (files == 0 || flags == 0)
		return (0);
	if (files == 1)
		ft_printf("%s:\n", av[file_ac]);
	return (files);
}
