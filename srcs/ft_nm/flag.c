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

int8_t		is_flag_nm(char *arg)
{
	size_t		i;

	if (arg[0] != '-')
		return (-1);
	i = 0;
	while (arg[++i] != '\0')
	{
		if (arg[i] == 'p')
			continue ;
		if (arg[i] == 'r')
			continue ;
		return (-2);
	}
	return (1);
}

int8_t		get_flag_nm(int8_t flag[2], int ac, char **av)
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
			if (av[i - 1][j] == 'p')
			{
				flag[0] = 1;
				continue ;
			}
			if (av[i - 1][j] == 'r')
			{
				flag[1] = 1;
				continue ;
			}
			return (0);
		}
	}
	return (1);
}

int8_t		verif_args_files_and_get_nb_files(int ac, char **av)
{
	int			i;
	int			files;
	int			file_ac;
	int8_t		res;

	files = 0;
	i = 1;
	while (i++ < ac)
	{
		if ((res = is_flag_nm(av[i - 1])) == -2)
		{
			return (-1);
		}
		else if (res == -1)
		{
			file_ac = i - 1;
			files++;
		}
	}
	return (files);
}
