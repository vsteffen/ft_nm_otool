#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		handle_32(char *content)
{
	struct s_nm_32				*nm_32;

	// print_segments_32_deprecated(content);
	nm_32 = get_nm_32(content);
	sort_nm_32(nm_32, 0);
	if (nm_32->sym_list)
		return (iter_sym_table_and_print_32(nm_32));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_64(char *content)
{
	struct s_nm_64				*nm_64;

	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64(content);
	sort_nm_64(nm_64, 0);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print_64(nm_64));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_fat_32(char *content)
{
	(void)content;
	return (-1);
}

int8_t		analyse_magic_number(char *content, char *path)
{
	unsigned int	magic_number;

	if (!content)
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
		return (-1);
	}
	magic_number = *(unsigned int *)content;
	if (magic_number == MH_MAGIC_64 || magic_number == MH_CIGAM_64)
	{
		return (handle_64(content));
	}
	else if (magic_number == MH_MAGIC || magic_number == MH_CIGAM)
	{
		return (handle_32(content));
	}
	else if (magic_number == FAT_MAGIC || magic_number == FAT_CIGAM)
	{
		ft_printf("ft_nm: %s: There is an fat 32 header ... a small one\n", path);
		return (handle_fat_32(content));
	}
	else if (magic_number == FAT_MAGIC_64 || magic_number == FAT_CIGAM_64)
	{
		ft_printf("ft_nm: %s: There is an fat 64 header ... a big one\n", path);
	}
	ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
	return (-1);
}

int8_t		get_file_content(char *path, int ac)
{
	int				fd;
	char			*content;
	struct stat		file_stat;

	if ((fd = open(path, O_RDONLY)) < 0)
		return (exit_nm(path, ": No such file or permission denied\n"));
	if (fstat(fd, &file_stat) < 0)
		return (exit_nm(path, ": fstat failed.\n"));
	if (S_ISDIR(file_stat.st_mode))
		return (exit_nm(path, ": Is a directory\n"));
	if ((content = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (exit_nm(path, ": Can't map file\n"));
	if (ac > 2)
		ft_printf("\n%s:\n", path);
	if (analyse_magic_number(content, path) == -1)
		return (EXIT_FAILURE);
	if (munmap(content, file_stat.st_size) < 0)
		return (exit_nm(path, ": Can't free memory allocated to map file\n"));
	if (close(fd) < 0)
		return (exit_nm(path, ": Error when closing file\n"));
	return (EXIT_SUCCESS);
}

int			main(int ac, char **av)
{
	int			i;
	int8_t		status;
	int8_t		return_status;

	if (ac == 1)
	{
		return (get_file_content("./a.out", 1));
	}
	else
	{
		i = 1;
		return_status = EXIT_SUCCESS;
		while (i++ < ac)
		{
			if ((status = get_file_content(av[i - 1], ac)) == EXIT_FAILURE)
				return_status = EXIT_FAILURE;
		}
		return (return_status);
	}
}
