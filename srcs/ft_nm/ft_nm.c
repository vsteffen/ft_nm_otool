#include "ft_nm_otool.h"

int8_t		exit_err(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

void		free_nm_32(struct s_nm_32 *nm_32)
{
	struct s_sym_32		*sym_32_tmp;
	struct s_sect_32	*sect_32_tmp;

	if (nm_32 && nm_32->sym_list)
	{
		while (nm_32->sym_list)
		{
			sym_32_tmp = nm_32->sym_list;
			nm_32->sym_list = nm_32->sym_list->next;
			free(sym_32_tmp);
		}
		while (nm_32->sect_list)
		{
			sect_32_tmp = nm_32->sect_list;
			nm_32->sect_list = nm_32->sect_list->next;
			free(sect_32_tmp);
		}
	}
	if (nm_32)
		free(nm_32);
}

void		free_nm_64(struct s_nm_64 *nm_64)
{
	struct s_sym_64		*sym_64_tmp;
	struct s_sect_64	*sect_64_tmp;

	if (nm_64 && nm_64->sym_list)
	{
		while (nm_64->sym_list)
		{
			sym_64_tmp = nm_64->sym_list;
			nm_64->sym_list = nm_64->sym_list->next;
			free(sym_64_tmp);
		}
		while (nm_64->sect_list)
		{
			sect_64_tmp = nm_64->sect_list;
			nm_64->sect_list = nm_64->sect_list->next;
			free(sect_64_tmp);
		}
	}
	if (nm_64)
		free(nm_64);
}

int8_t		handle_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	struct s_nm_64				*nm_64;
	int8_t						res;

	nm_64 = get_nm_64(ptr_header, endian);
	sort_nm_64(nm_64, endian, nm_data);
	if (nm_64->sym_list)
		res = iter_sym_table_and_print_64(nm_64, endian);
	else
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
		res = -1;
	}
	free_nm_64(nm_64);
	return (res);
}

int8_t		handle_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	struct s_nm_32				*nm_32;
	int8_t						res;

	nm_32 = get_nm_32(ptr_header, endian);
	sort_nm_32(nm_32, endian, nm_data);
	if (nm_32->sym_list)
		res = iter_sym_table_and_print_32(nm_32, endian);
	else
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
		res = -1;
	}
	free_nm_32(nm_32);
	return (res);
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

int8_t		handle_fat_header_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
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
	while (i < nb_arch)
	{
		fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_32->cputype) : fat_arch_32->cputype));
		offset_arch = (endian) ? endian_swap_int32(fat_arch_32->offset) : fat_arch_32->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, nm_data);
		i++;
	}
	return (1);
}

int8_t		handle_fat_header_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
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
	while (i < nb_arch)
	{
		fat_arch_64 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_64->cputype) : fat_arch_64->cputype));
		offset_arch = (endian) ? endian_swap_int64(fat_arch_64->offset) : fat_arch_64->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, nm_data);
		i++;
	}
	return (1);
}

void		*find_next_ar_header(void *ptr_header, size_t *i, struct s_nm_data *nm_data)
{
	char		*file;

	file = (char *)ptr_header;
	while (*i < nm_data->file_size)
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

void		*find_begin_ar_file(void *ptr_header, size_t *i, struct s_nm_data *nm_data)
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

uint64_t		get_ran_off_64(void *ptr_header, size_t *i, struct s_nm_data *nm_data)
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

uint32_t		get_ran_off_32(void *ptr_header, size_t *i, struct s_nm_data *nm_data)
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

int8_t		handle_ar(void *ptr_header, struct s_nm_data *nm_data)
{
	uint64_t			ran_off;
	int8_t				is_32_ar;
	size_t				i;
	size_t				j;
	size_t				symtab_pos;
	uint64_t			offset_file_tmp;

	i = 0;
	is_32_ar = 0;
	find_next_ar_header(ptr_header, &i, nm_data);
	if (ft_strncmp(SYMDEF, ptr_header + i, ft_strlen(SYMDEF)) == 0 || ft_strncmp(SYMDEF_SORTED, ptr_header + i, ft_strlen(SYMDEF_SORTED)) == 0)
	{
		is_32_ar = 1;
		ran_off = get_ran_off_32(ptr_header, &i, nm_data);
	}
	else if (ft_strncmp(SYMDEF_64, ptr_header + i, ft_strlen(SYMDEF_64)) == 0 || ft_strncmp(SYMDEF_64_SORTED, ptr_header + i, ft_strlen(SYMDEF_64_SORTED)) == 0)
		ran_off = get_ran_off_64(ptr_header, &i, nm_data);
	else
		return (1);
	if (ran_off == 0)
		return (1);
	symtab_pos = i;
	j = i + ((is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	offset_file_tmp = (uint64_t) - 1;
	while (j < ran_off + symtab_pos)
	{
		i = (is_32_ar) ? *(uint32_t*)(ptr_header + j) : *(uint64_t*)(ptr_header + symtab_pos + j);
		if (i >= nm_data->file_size)
			return (1);
		find_next_ar_header(ptr_header, &i, nm_data);
		if (offset_file_tmp == (uint64_t) - 1 || i != offset_file_tmp)
		{
			offset_file_tmp = i;
			ft_printf("\n%s(%s):\n", nm_data->file_path, ptr_header + i);
			find_begin_ar_file(ptr_header, &i, nm_data);
			match_and_use_magic_number(ptr_header + i, *(uint32_t *)(ptr_header + i), nm_data);
		}
		j += 2 * ((is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	}
	return (0);
}

int8_t		match_and_use_magic_number(void *ptr_header, uint32_t magic_number, struct s_nm_data *nm_data)
{
	if (magic_number == MH_MAGIC_64)
		return (handle_64(ptr_header, 0, nm_data));
	else if (magic_number == MH_CIGAM_64)
		return (handle_64(ptr_header, 1, nm_data));
	else if (magic_number == MH_MAGIC)
		return (handle_32(ptr_header, 0, nm_data));
	else if (magic_number == MH_CIGAM)
		return (handle_32(ptr_header, 1, nm_data));
	else if (magic_number == AR_MAGIC)
		return (handle_ar(ptr_header, nm_data));
	ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
	return (-2);
}

int8_t		analyse_magic_number(void *ptr_header, struct s_nm_data *nm_data)
{
	uint32_t		magic_number;

	if (!ptr_header)
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
		return (-1);
	}
	magic_number = *(uint32_t *)ptr_header;
	if (magic_number == FAT_MAGIC)
		return (handle_fat_header_32(ptr_header, 0, nm_data));
	else if (magic_number == FAT_CIGAM)
		return (handle_fat_header_32(ptr_header, 1, nm_data));
	else if (magic_number == FAT_MAGIC_64)
		return (handle_fat_header_64(ptr_header, 0, nm_data));
	else if (magic_number == FAT_CIGAM_64)
		return (handle_fat_header_64(ptr_header, 1, nm_data));
	return (match_and_use_magic_number(ptr_header, magic_number, nm_data));
}

int8_t		get_file_content(char *path, int nb_files, struct s_nm_data *nm_data)
{
	int					fd;
	char				*content;
	struct stat			file_stat;

	if ((fd = open(path, O_RDONLY)) < 0)
		return (exit_err(path, ": No such file or permission denied\n"));
	if (fstat(fd, &file_stat) < 0)
		return (exit_err(path, ": fstat failed.\n"));
	if (S_ISDIR(file_stat.st_mode))
		return (exit_err(path, ": Is a directory\n"));
	nm_data->file_path = path;
	nm_data->file_size = file_stat.st_size;
	if ((content = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) \
		== MAP_FAILED)
		return (exit_err(path, ": Can't map file\n"));
	if (nb_files > 1)
		ft_printf("\n%s:\n", path);
	if (analyse_magic_number((void*)content, nm_data) == -1)
		return (EXIT_FAILURE);
	if (munmap(content, file_stat.st_size) < 0)
		return (exit_err(path, ": Can't free memory allocated to map file\n"));
	if (close(fd) < 0)
		return (exit_err(path, ": Error when closing file\n"));
	return (EXIT_SUCCESS);
}

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

int8_t			verif_args_files_and_get_nb_files(int ac, char **av)
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

int			main(int ac, char **av)
{
	int					i;
	struct s_status		status;
	struct s_nm_data	nm_data;
	int8_t				flag[2];
	int					nb_files;

	nm_data.flag = flag;
	ft_bzero(nm_data.flag, sizeof(int8_t) * 2);
	if ((nb_files = verif_args_files_and_get_nb_files(ac, av)) < 0 || \
		get_flag_nm(nm_data.flag, ac, av) == 0)
		return (exit_err("", "Unknown command line argument\n"));
	if (nb_files == 0)
		return (get_file_content("a.out", 1, &nm_data));
	status.ret = EXIT_SUCCESS;
	i = 1;
	while (i++ < ac)
	{
		if ((status.res = is_flag_nm(av[i - 1])) < 0)
		{
			if ((status.res = get_file_content(av[i - 1], nb_files, \
				&nm_data)) == EXIT_FAILURE)
				status.ret = EXIT_FAILURE;
		}
	}
	return (status.ret);
}
