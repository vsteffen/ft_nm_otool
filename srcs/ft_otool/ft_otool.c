/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_otool.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 00:04:58 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/23 00:05:00 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm_otool.h"

int8_t	exit_err(char *path, char *message)
{
	ft_putstr("ft_otool: ");
	ft_putstr(path);
	ft_putstr(message);
	ft_putstr(".\n");
	return (EXIT_FAILURE);
}

int8_t	get_file_content(char *path, int nb_files, struct s_nm_data *nm_data)
{
	int					fd;
	char				*content;
	struct stat			file_stat;

	if ((fd = open(path, O_RDONLY)) < 0)
		return (exit_err(path, ": No such file or permission denied"));
	if (fstat(fd, &file_stat) < 0)
		return (exit_err(path, ": fstat failed"));
	if (S_ISDIR(file_stat.st_mode))
		return (exit_err(path, ": Is a directory"));
	if ((content = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) \
		== MAP_FAILED)
		return (exit_err(path, ": Can't map file"));
	if (nb_files > 1)
		ft_printf("%s:\n", path);
	nm_data->file_path = path;
	nm_data->file_size = file_stat.st_size;
	if (analyse_magic_number((void*)content, nm_data) == -1)
		return (EXIT_FAILURE);
	if (munmap(content, file_stat.st_size) < 0)
		return (exit_err(path, ": Can't free memory allocated to map file"));
	if (close(fd) < 0)
		return (exit_err(path, ": Error when closing file"));
	return (EXIT_SUCCESS);
}

int8_t	exit_usage(void)
{
	ft_putstr("Usage: ft_otool: [-dt] <object file> ...\n\t\
-t Display the contents of the (__TEXT,__text) section\n\t\
-d Display the contents of the (__DATA,__data) section\n");
	return (EXIT_FAILURE);
}

int		main(int ac, char **av)
{
	int					i;
	struct s_status		status;
	struct s_nm_data	nm_data;
	int8_t				flag[2];
	int					nb_files;

	nm_data.flag = flag;
	ft_bzero(nm_data.flag, sizeof(int8_t) * 2);
	if (get_flag_otool(nm_data.flag, ac, av) == 0 || \
		(nb_files = verif_files_and_print_filename_if_one_file(ac, av)) == 0)
		return (exit_usage());
	status.ret = EXIT_SUCCESS;
	i = 1;
	while (i++ < ac)
	{
		if ((status.res = is_flag_otool(av[i - 1])) < 0)
		{
			if ((status.res = get_file_content(av[i - 1], nb_files, &nm_data)) \
				== EXIT_FAILURE)
				status.ret = EXIT_FAILURE;
		}
	}
	return (status.ret);
}
