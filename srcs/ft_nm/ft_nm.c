#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		handle_64(void *ptr_header, int8_t endian, int8_t flag[3])
{
	struct s_nm_64				*nm_64;

	// ft_printf("Handle 64 bits in %s endian\n", (endian) ? "little" : "big");
	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64(ptr_header, endian);
	sort_nm_64(nm_64, endian, flag);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print_64(nm_64, endian));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_32(void *ptr_header, int8_t endian, int8_t flag[3])
{
	struct s_nm_32				*nm_32;

	// ft_printf("Handle 32 bits in %s endian\n", (endian) ? "little" : "big");
	// print_segments_32_deprecated(content);
	nm_32 = get_nm_32(ptr_header, endian);
	sort_nm_32(nm_32, endian, flag);
	if (nm_32->sym_list)
		return (iter_sym_table_and_print_32(nm_32, endian));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int32_t		endian_swap_int32(uint32_t x)
{
	x = (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | \
		(x << 24);
	return (x);
}

int64_t		endian_swap_int64(uint64_t x)
{
	x = (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | \
		(x << 24);
	return (x);
}

char		*get_architecture_name(cpu_type_t cputype)
{
	if (cputype == CPU_TYPE_VAX)
		return ("VAX");
	if (cputype == CPU_TYPE_MC680x0)
		return ("MC680x0");
	if (cputype == CPU_TYPE_X86 || cputype == CPU_TYPE_I386)
		return ("i386");
	if (cputype == CPU_TYPE_X86_64)
		return ("x86_64");
	if (cputype == CPU_TYPE_MC98000)
		return ("MC98000");
	if (cputype == CPU_TYPE_HPPA)
		return ("HPPA");
	if (cputype == CPU_TYPE_ARM)
		return ("ARM");
	if (cputype == CPU_TYPE_ARM64)
		return ("ARM64");
	if (cputype == CPU_TYPE_MC88000)
		return ("MC88000");
	if (cputype == CPU_TYPE_SPARC)
		return ("SPARC");
	if (cputype == CPU_TYPE_I860)
		return ("i860");
	if (cputype == CPU_TYPE_POWERPC)
		return ("ppc");
	if (cputype == CPU_TYPE_POWERPC64)
		return ("ppc64");
	return ("Unknown");
}

int8_t		handle_fat_header_32(void *ptr_header, int8_t endian, struct s_file_st *file_st, int8_t flag[3])
{
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_32;
	uint32_t			nb_arch;
	uint32_t			offset_arch;
	uint32_t			magic_number;
	size_t				i;

	fat_header = (struct fat_header*)ptr_header;
	fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header));
	nb_arch = (endian) ? endian_swap_int32(fat_header->nfat_arch) : fat_header->nfat_arch;
	i = 0;
	// ft_printf("ft_nm: fat_header 32 bits [%X] with [%zu] architectures\n", (endian == 0) ? FAT_MAGIC : FAT_CIGAM, nb_arch);
	while (i < nb_arch)
	{
		fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", file_st->path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_32->cputype) : fat_arch_32->cputype));
		offset_arch = (endian) ? endian_swap_int32(fat_arch_32->offset) : fat_arch_32->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		// ft_printf(" └─> Magic_number [%X] for architecture n˚[%X]\n", magic_number, i);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, file_st, flag);
		i++;
	}
	return (1);
}

int8_t		handle_fat_header_64(void *ptr_header, int8_t endian, struct s_file_st *file_st, int8_t flag[3])
{
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_64;
	uint32_t			nb_arch;
	uint32_t			offset_arch;
	uint32_t			magic_number;
	size_t				i;

	fat_header = (struct fat_header*)ptr_header;
	fat_arch_64 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header));
	nb_arch = (endian) ? endian_swap_int32(fat_header->nfat_arch) : fat_header->nfat_arch;
	i = 0;
	// ft_printf("ft_nm: fat_header 32 bits [%X] with [%zu] architectures\n", (endian == 0) ? FAT_MAGIC : FAT_CIGAM, nb_arch);
	while (i < nb_arch)
	{
		fat_arch_64 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", file_st->path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_64->cputype) : fat_arch_64->cputype));
		offset_arch = (endian) ? endian_swap_int64(fat_arch_64->offset) : fat_arch_64->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		// ft_printf(" └─> Magic_number [%X] for architecture n˚[%X]\n", magic_number, i);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, file_st, flag);
		i++;
	}
	return (1);
}

void		*find_next_ar_header(void *ptr_header, size_t *i, struct s_file_st *file_st)
{
	char		*file;

	file = (char *)ptr_header;
	while (*i < file_st->size)
	{
		if (ft_strncmp(ARFMAG, file + *i, ft_strlen(ARFMAG)) == 0)
		{
			*i += 2;
			return ((void*)ptr_header + *i);
		}
		(*i)++;
	}
	return (NULL);
}

void		*find_begin_ar_file(void *ptr_header, size_t *i, struct s_file_st *file_st)
{
	char		*file;

	// ft_printf("i = [%zu]\n", *i);
	file = (char *)ptr_header;
	while (file[*i] != '\0')
		(*i)++;
	*i += sizeof(uint32_t) - (*i % sizeof(uint32_t));
	while (*i < file_st->size)
	{
		if (*(uint32_t*)(ptr_header + *i) > 0)
			return (ptr_header + *i);
		*i += sizeof(uint32_t);
	}
	return (NULL);
}

uint64_t		get_ran_off_64(void *ptr_header, size_t *i, struct s_file_st *file_st)
{
	*i += ft_strlen((char*)(ptr_header + *i));
	*i += sizeof(uint64_t) - (*i % sizeof(uint64_t));
	while (*i < file_st->size)
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

uint32_t		get_ran_off_32(void *ptr_header, size_t *i, struct s_file_st *file_st)
{
	*i += ft_strlen((char*)(ptr_header + *i));
	*i += sizeof(uint32_t) - (*i % sizeof(uint32_t));
	while (*i < file_st->size)
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

int8_t		handle_ar(void *ptr_header, struct s_file_st *file_st, int8_t flag[3])
{
	uint64_t			ran_off;
	int8_t				is_32_ar;
	size_t				i;
	size_t				j;
	size_t				symtab_pos;
	// uint64_t			offset_file;
	uint64_t			offset_file_tmp;

	i = 0;
	is_32_ar = 0;
	find_next_ar_header(ptr_header, &i, file_st);
	if (ft_strncmp(SYMDEF, ptr_header + i, ft_strlen(SYMDEF)) == 0 || ft_strncmp(SYMDEF_SORTED, ptr_header + i, ft_strlen(SYMDEF_SORTED)) == 0)
	{
		is_32_ar = 1;
		ran_off = get_ran_off_32(ptr_header, &i, file_st);
	}
	else if (ft_strncmp(SYMDEF_64, ptr_header + i, ft_strlen(SYMDEF_64)) == 0 || ft_strncmp(SYMDEF_64_SORTED, ptr_header + i, ft_strlen(SYMDEF_64_SORTED)) == 0)
		ran_off = get_ran_off_64(ptr_header, &i, file_st);
	else
		return (1);
	if (ran_off == 0)
		return (1);
	// ft_printf("ran_off -> [%zu] and i [0x%zu]\n", ran_off, i);
	symtab_pos = i;
	j = i + ((is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	offset_file_tmp = (uint64_t) - 1;
	while (j < ran_off + symtab_pos)
	{
		// ft_printf("j [%zu] -> offset -> [%X]\n", j, *(size_t*)(ptr_header + j));
		i = (is_32_ar) ? *(uint32_t*)(ptr_header + j) : *(uint64_t*)(ptr_header + symtab_pos + j);
		// ft_printf("New assign to i [0x%X]\n", i);
		// ft_printf("New assign to i [0x%zu] | string -> [%s]\n", i, ptr_header + i);
		if (i >= file_st->size)
			return (1);
		find_next_ar_header(ptr_header, &i, file_st);
		// ft_printf("Second new assign to i [0x%X]\n", i);
		if (offset_file_tmp == (uint64_t) - 1 || i != offset_file_tmp)
		{
			offset_file_tmp = i;
			ft_printf("\n\ni vaut [%zu]", i);
			ft_printf("\n%s(%s):\n", file_st->path, ptr_header + i);
			find_begin_ar_file(ptr_header, &i, file_st);
			match_and_use_magic_number(ptr_header + i, *(uint32_t *)(ptr_header + i), file_st, flag);
		}
		// ft_printf("Third new assign to i [0x%X]\n", i);
		// exit(0);
		// (void)flag;
		// ft_printf("j -> [%X]\n", j);
		j += 2 * ((is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	}
	// ft_printf("End of while, j -> [%X]\n", j);
	return (0);
}

int8_t		match_and_use_magic_number(void *ptr_header, uint32_t magic_number, struct s_file_st *file_st, int8_t flag[3])
{
	if (magic_number == MH_MAGIC_64)
		return (handle_64(ptr_header, 0, flag));
	else if (magic_number == MH_CIGAM_64)
		return (handle_64(ptr_header, 1, flag));
	else if (magic_number == MH_MAGIC)
		return (handle_32(ptr_header, 0, flag));
	else if (magic_number == MH_CIGAM)
		return (handle_32(ptr_header, 1, flag));
	else if (magic_number == AR_MAGIC)
		return (handle_ar(ptr_header, file_st, flag));
	ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", file_st->path);
	return (-2);
}

int8_t		analyse_magic_number(void *ptr_header, char *path, int8_t flag[3], struct s_file_st *file_st)
{
	uint32_t		magic_number;

	if (!ptr_header)
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
		return (-1);
	}
	magic_number = *(uint32_t *)ptr_header;
	if (magic_number == FAT_MAGIC)
		return (handle_fat_header_32(ptr_header, 0, file_st, flag));
	else if (magic_number == FAT_CIGAM)
		return (handle_fat_header_32(ptr_header, 1, file_st, flag));
	else if (magic_number == FAT_MAGIC_64)
		return (handle_fat_header_64(ptr_header, 0, file_st, flag));
	else if (magic_number == FAT_CIGAM_64)
		return (handle_fat_header_64(ptr_header, 1, file_st, flag));
	return (match_and_use_magic_number(ptr_header, magic_number, file_st, flag));
}

void		init_file_st(struct s_file_st *file_st, char *path, size_t st_size)
{
	file_st->path = path;
	file_st->size = st_size;
}

int8_t		get_file_content(char *path, int ac, int8_t flag[3])
{
	int					fd;
	char				*content;
	struct stat			file_stat;
	struct s_file_st	file_st;

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
	init_file_st(&file_st, path, file_stat.st_size);
	if (analyse_magic_number((void*)content, path, flag, &file_st) == -1)
		return (EXIT_FAILURE);
	if (munmap(content, file_stat.st_size) < 0)
		return (exit_nm(path, ": Can't free memory allocated to map file\n"));
	if (close(fd) < 0)
		return (exit_nm(path, ": Error when closing file\n"));
	return (EXIT_SUCCESS);
}

int8_t		is_flag(char *arg)
{
	if (arg[0] != '-')
		return (-1);
	if (arg[1] == 'p' && arg[2] == '\0')
		return (0);
	if (arg[1] == 'r' && arg[2] == '\0')
		return (1);
	if (arg[1] == 'm' && arg[2] == '\0')
		return (2);
	return (-2);
}

void		get_flag(int8_t flag[3], int ac, char **av)
{
	int			i;
	int8_t		res;

	ft_bzero(flag, sizeof(int8_t) * 3);
	i = 1;
	while (i++ < ac)
	{
		if ((res = is_flag(av[i - 1])) != -1)
			flag[res] = 1;
	}
}

int			main(int ac, char **av)
{
	int			i;
	int8_t		status;
	int8_t		return_status;
	int8_t		flag[3];
	int8_t		arg_path_found;

	arg_path_found = 0;
	get_flag(flag, ac, av);
	return_status = EXIT_SUCCESS;
	i = 1;
	while (i++ < ac)
	{
		if ((status = is_flag(av[i - 1])) < 0)
		{
			if (status == -2)
				return (exit_nm("Unknown command line argument '", ft_strjoin(av[i - 1], "'\n")));
			arg_path_found = 1;
			if ((status = get_file_content(av[i - 1], ac, flag)) == EXIT_FAILURE)
				return_status = EXIT_FAILURE;
		}
	}
	if (arg_path_found == 0)
		return (get_file_content("a.out", 1, flag));
	return (return_status);
}
