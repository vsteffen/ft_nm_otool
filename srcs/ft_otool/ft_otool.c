#include "ft_nm_otool.h"

int8_t		exit_err(char *path, char *message)
{
	ft_putstr("ft_otool: ");
	ft_putstr(path);
	ft_putstr(message);
	ft_putstr(".\n");
	return (EXIT_FAILURE);
}

int8_t		handle_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	return (find_segments_and_sections_64(ptr_header, endian, nm_data));
}

int8_t		handle_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	return (find_segments_and_sections_32(ptr_header, endian, nm_data));
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

	if (sizeof(struct fat_header) + sizeof(struct fat_arch) >= \
		nm_data->file_size)
	return (exit_err(nm_data->file_path, " corrupted binary"));
	fat_header = (struct fat_header*)ptr_header;
	fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header));
	nb_arch = (endian) ? endian_swap_int32(fat_header->nfat_arch) : fat_header->nfat_arch;
	i = 0;
	while (i < nb_arch)
	{
		if (sizeof(struct fat_header) + sizeof(struct fat_arch) * i + sizeof(struct fat_arch) >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_32->cputype) : fat_arch_32->cputype));
		offset_arch = (endian) ? endian_swap_int32(fat_arch_32->offset) : fat_arch_32->offset;
		if (offset_arch >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, nm_data);
		i++;
	}
	return (0);
}

int8_t		handle_fat_header_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data)
{
	struct fat_header	*fat_header;
	struct fat_arch_64	*fat_arch_64;
	uint32_t			nb_arch;
	uint32_t			offset_arch;
	uint32_t			magic_number;
	size_t				i;

	if (sizeof(struct fat_header) + sizeof(struct fat_arch_64) >= \
		nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	fat_header = (struct fat_header*)ptr_header;
	fat_arch_64 = (struct fat_arch_64*)(ptr_header + sizeof(struct fat_header));
	nb_arch = (endian) ? endian_swap_int32(fat_header->nfat_arch) : fat_header->nfat_arch;
	i = 0;
	while (i < nb_arch)
	{
		if (sizeof(struct fat_header) + sizeof(struct fat_arch) * i + sizeof(struct fat_arch_64) >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		fat_arch_64 = (struct fat_arch_64*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		ft_printf("\n%s (for architecture %s):\n", nm_data->file_path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_64->cputype) : fat_arch_64->cputype));
		offset_arch = (endian) ? endian_swap_int64(fat_arch_64->offset) : fat_arch_64->offset;
		if (offset_arch >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, nm_data);
		i++;
	}
	return (0);
}

void		find_next_ar_header(void *ptr_header, size_t *i, struct s_nm_data *nm_data)
{
	char		*file;

	file = (char *)ptr_header;
	while (*i < nm_data->file_size)
	{
		if (ft_strncmp(ARFMAG, file + *i, ft_strlen(ARFMAG)) == 0)
		{
			*i += 2;
			return ;
		}
		(*i)++;
	}
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
	if (i >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	if (ft_strncmp(SYMDEF, ptr_header + i, ft_strlen(SYMDEF)) == 0 || ft_strncmp(SYMDEF_SORTED, ptr_header + i, ft_strlen(SYMDEF_SORTED)) == 0)
	{
		is_32_ar = 1;
		ran_off = get_ran_off_32(ptr_header, &i, nm_data);
	}
	else if (ft_strncmp(SYMDEF_64, ptr_header + i, ft_strlen(SYMDEF_64)) == 0 || ft_strncmp(SYMDEF_64_SORTED, ptr_header + i, ft_strlen(SYMDEF_64_SORTED)) == 0)
		ran_off = get_ran_off_64(ptr_header, &i, nm_data);
	else
		return (1);
	symtab_pos = i;
	if (ran_off == 0 || ran_off + symtab_pos >= nm_data->file_size)
		return (exit_err(nm_data->file_path, " corrupted binary"));
	j = i + ((is_32_ar) ? sizeof(uint32_t) : sizeof(uint64_t));
	offset_file_tmp = (uint64_t)-1;
	while (j < ran_off + symtab_pos)
	{
		i = (is_32_ar) ? *(uint32_t*)(ptr_header + j) : *(uint64_t*)(ptr_header + symtab_pos + j);
		if (i >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		find_next_ar_header(ptr_header, &i, nm_data);
		if (i >= nm_data->file_size)
			return (exit_err(nm_data->file_path, " corrupted binary"));
		if (offset_file_tmp == (uint64_t)-1 || i != offset_file_tmp)
		{
			offset_file_tmp = i;
			ft_printf("\n%s(%s):\n", nm_data->file_path, ptr_header + i);
			find_begin_ar_file(ptr_header, &i, nm_data);
			if (i >= nm_data->file_size)
				return (exit_err(nm_data->file_path, " corrupted binary"));
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
	ft_printf("ft_otool: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
	return (-2);
}

int8_t		analyse_magic_number(void *ptr_header, struct s_nm_data *nm_data)
{
	uint32_t		magic_number;

	if (!ptr_header)
	{
		ft_printf("ft_otool: %s: The file was not recognized as a valid object file\n", nm_data->file_path);
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
		return (exit_err(path, ": No such file or permission denied"));
	if (fstat(fd, &file_stat) < 0)
		return (exit_err(path, ": fstat failed"));
	if (S_ISDIR(file_stat.st_mode))
		return (exit_err(path, ": Is a directory"));
	if ((content = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
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

int8_t			verif_files_and_print_filename_if_one_file(int ac, char **av)
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

int8_t			exit_usage(void)
{
	ft_putstr("Usage: ft_otool: [-dt] <object file> ...\n\t\
-t Display the contents of the (__TEXT,__text) section\n\t\
-d Display the contents of the (__DATA,__data) section\n");
	return (EXIT_FAILURE);
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
	if (get_flag_otool(nm_data.flag, ac, av) == 0 || \
		(nb_files = verif_files_and_print_filename_if_one_file(ac, av)) == 0)
		return (exit_usage());
	status.ret = EXIT_SUCCESS;
	i = 1;
	while (i++ < ac)
	{
		if ((status.res = is_flag_otool(av[i - 1])) < 0)
		{
			if ((status.res = get_file_content(av[i - 1], nb_files, &nm_data)) == EXIT_FAILURE)
				status.ret = EXIT_FAILURE;
		}
	}
	return (status.ret);
}
