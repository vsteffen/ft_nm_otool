#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		iter_sym_table_and_print(uint32_t nsyms, uint32_t symoff, uint32_t stroff, char *content)
{
	uint32_t			i;
	uint32_t			count;
	char				*string_table;
	struct nlist_64		*sym_table_entry;

	sym_table_entry = (void *)content + symoff;
	string_table = (void *)content + stroff;
	i = 0;
	count = 0;
	while (i < nsyms)
	{
		// ft_printf("sym_table_entry[i].n_type -> %x | (sym_table_entry[i].n_type & N_STAB) -> %x | N_STAB -> %x\n", sym_table_entry[i].n_type, (sym_table_entry[i].n_type & N_STAB), N_STAB);
		if ((sym_table_entry[i].n_type & N_STAB) == 0)
		{
			// ft_printf("%d: [%s] | n_type -> [%hhx] | n_sect -> [%hhx]\n"
			// , i, string_table + sym_table_entry[i].n_un.n_strx, sym_table_entry[i].n_type, sym_table_entry[i].n_sect);
			ft_printf("%016llx", sym_table_entry[i].n_value);
			if ((sym_table_entry[i].n_type & N_UNDF) == N_UNDF)
				ft_printf(" U ");
			else if ((sym_table_entry[i].n_type & N_ABS) == N_ABS)
				ft_printf(" A ");
			else if ((sym_table_entry[i].n_type & N_PBUD) == N_PBUD)
				ft_printf(" u ");
			else if ((sym_table_entry[i].n_type & N_INDR) == N_INDR)
				ft_printf(" U ");
			else
			{
				ft_putstr("ft_nm: Unknown symbol table type\n");
				return (-1);
			}
			ft_printf("%s\n", string_table + sym_table_entry[i].n_un.n_strx);
			count++;
		}
		// ft_putstr(string_table + array[i].n_un.n_strx);
		// ft_putchar('\n');
		i++;
	}
	return (0);
	// ft_printf("Count -> %d\n", count);
}

int8_t		handle_64(char *content)
{
	uint32_t				ncmds;
	size_t					i;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	header = (struct mach_header_64 *)content;
	ncmds = header->ncmds;
	lc = (void *)content + sizeof(*header);
	i = 0;
	while (i++ < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			// ft_printf("NB symbole -> %d\n", sym->nsyms);
			return (iter_sym_table_and_print(sym->nsyms, sym->symoff, sym->stroff, content));

		}
		lc = (void *)lc + lc->cmdsize;
	}
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		analyse_content(char *content)
{
	unsigned int	magic_number;

	magic_number = *(unsigned int *)content;
	if (magic_number == MH_MAGIC_64)
	{
		// ft_putstr("This is an mach-o 64 file\n");
		return (handle_64(content));
	}
	else
	{
		// ft_putstr("This isn't an mach-o 64 file\n");
	}
	return (handle_64(content));
	ft_putstr("ft_nm: Unknown magic number\n");
	return (-1);
}

int8_t		get_file_content(char *path)
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
	if (analyse_content(content) == -1)
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
