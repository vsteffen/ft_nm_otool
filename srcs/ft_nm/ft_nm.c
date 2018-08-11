#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		get_location(int8_t n_type)
{
	if ((n_type & N_STAB) > 0)
		return (0); // symbol is for debugging
	if ((n_type & N_EXT) != N_EXT && (n_type & N_PEXT) != N_PEXT)
		return (1); // symbol is non external
	if ((n_type & N_EXT) == N_EXT)
		return (2); // symbol is external
	return (3); // symbol is private external
}

char		get_correct_letter(char letter, int8_t stab_ext_p_local)
{
	if (stab_ext_p_local == 1)
		return (letter + 32);
	return (letter);
}

struct s_sect_64		*get_section_64(struct s_sect_64 *sect_list, uint32_t ordinal)
{
	while (sect_list)
	{
		if (sect_list->ordinal == ordinal)
			return (sect_list);
		sect_list = sect_list->next;
	}
	return (NULL);
}

// CHEAT SHEET

// debug -> mask N_STAB on n_type
// external -> mask N_EXT on n_type
// private external -> mask N_PEXT on n_type
// non external -> mask N_EXT and N_PEXT on n_type

// Get true type with mask N_TYPE on n_type
// undefined -> mask N_UNDF with N_TYPE
// absolute -> mask N_ABS with N_TYPE
// indirect -> mask N_INDR with N_TYPE
// section -> mask N_SECT with N_TYPE
// common -> masks N_UNDF and N_EXT with n_value > 0

int8_t		iter_sym_table_and_print(struct s_nm_64 *nm_64)
{
	struct s_sym_64		*sym;
	struct s_sect_64	*sect;
	int8_t				stab_ext_p_local;
	int8_t				real_type;

	sym = nm_64->sym_list;
	while (sym)
	{
		stab_ext_p_local = get_location(sym->elem.n_type);
		real_type = (sym->elem.n_type & N_TYPE);
		if (stab_ext_p_local > 0)
		{
			if (real_type != N_UNDF)
				ft_printf("%016llx", sym->elem.n_value);
			else
				ft_putstr("                ");
			// if (stab_ext_p_local == 2 && (real_type & N_UNDF) == N_UNDF && sym->elem.n_value > 0) // Detect common
			// 	ft_printf(" %c ", get_correct_letter('C', stab_ext_p_local));
			if ((real_type & N_SECT) == N_SECT) // section
			{
				if (!(sect = get_section_64(nm_64->sect_list, sym->elem.n_sect)))
				{
					ft_putstr("\nft_nm: Unknown section\n");
					return (-1);
				}

				// if (sec != NULL)
					// ft_printf(" (%s,%s) ", sect->elem.segname, sect->elem.sectname);

				if (ft_strcmp(sect->elem.sectname, SECT_TEXT) == 0)
					ft_printf(" %c ", get_correct_letter('T', stab_ext_p_local));
				else if (ft_strcmp(sect->elem.sectname, SECT_DATA) == 0)
					ft_printf(" %c ", get_correct_letter('D', stab_ext_p_local));
				else if (ft_strcmp(sect->elem.sectname, SECT_BSS) == 0)
					ft_printf(" %c ", get_correct_letter('B', stab_ext_p_local));
				else
					ft_printf(" %c ", get_correct_letter('S', stab_ext_p_local));
			}
			else if ((real_type & N_UNDF) == N_UNDF) // detect undefined
			{
					ft_printf(" %c ", get_correct_letter('U', stab_ext_p_local));
			}
			else if ((real_type & N_ABS) == N_ABS) // detect absolute
				ft_printf(" %c ", get_correct_letter('A', stab_ext_p_local));
			else if ((real_type & N_PBUD) == N_PBUD)
					ft_printf(" u "); //, get_correct_letter('U', stab_ext_p_local));
			else if ((real_type & N_INDR) == N_INDR) // detect indirect
				ft_printf(" %c ", get_correct_letter('I', stab_ext_p_local));
			else
			{
				ft_putstr("ft_nm: Unknown symbol table type\n");
				return (-1);
			}
			ft_printf("%s\n", sym->sym_table_string);
		}
		sym = sym->next;
	}
	return (0);
}

int8_t		handle_64(char *content)
{
	uint32_t					ncmds;
	size_t						i;
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct segment_command_64	*seg;
	struct s_nm_64				*nm_64;

	header = (struct mach_header_64 *)content;
	ncmds = header->ncmds;
	lc = (void *)content + sizeof(*header);
	i = 0;
	seg = NULL;
	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64(content);
	sort_nm_64(nm_64, 0);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print(nm_64));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		analyse_content(char *content, char *path)
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
		ft_putstr("This is an mach-o 32 bits file\n");
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
	if (analyse_content(content, path) == -1)
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
