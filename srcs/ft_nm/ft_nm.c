#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

void		print_line(uint32_t nsyms, uint32_t symoff, uint32_t stroff, char *content)
{
	size_t				i;
	char				*string_table;
	struct n_list_64	*array;

	array = (void *)content + symoff;
	string_table = (void *)content + stroff;
	i = 0;
	while (i++ < nsyms)
	{
		printf("%s\n", (void*)string_table + array[i].n_un.n_strx);
		// ft_putstr(string_table + array[i].n_un.n_strx);
		// ft_putchar('\n');
	}
}

void		handle_64(char *content)
{
	uint32_t				ncmds;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;
	size_t					i;

	header = (struct mach_header_64 *)content;
	ncmds = header->ncmds;
	lc = (void *)content + sizeof(header);
	i = 0;
	while (i++ < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			print_line(sym->nsyms, sym->symoff, sym->stroff, content);
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
	}
}

void		analyse_content(char *content)
{
	unsigned int	magic_number;

	magic_number = *(unsigned int *)content;
	if (magic_number == MH_MAGIC_64)
	{
		ft_putstr("This is an mach-o 64 file\n");
		handle_64(content);
	}
	else
		ft_putstr("This isn't an mach-o 64 file\n");
}

int8_t		get_file_content(char *path)
{
	int				fd;
	char			*content;
	struct stat		file_stat;

	// ft_putstr("nm -> ");
	// ft_putstr(path);
	// ft_putstr("\n");

	if ((fd = open(path, O_RDONLY)) < 0)
		return (exit_nm(path, ": No such file or permission denied\n"));
	if (fstat(fd, &file_stat) < 0)
		return (exit_nm(path, ": fstat failed.\n"));
	if (S_ISDIR(file_stat.st_mode))
		return (exit_nm(path, ": Is a directory\n"));
	if ((content = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (exit_nm(path, ": Can't map file\n"));
	analyse_content(content);
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
		return (get_file_content("./a.out"));
	}
	else
	{
		i = 1;
		return_status = EXIT_SUCCESS;
		while (i++ < ac)
		{
			if ((status = get_file_content(av[i - 1])) == EXIT_FAILURE)
				return_status = EXIT_FAILURE;
		}
		return (return_status);
	}
}
