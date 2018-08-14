#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		handle_64(void *ptr_header, int8_t endian)
{
	struct s_nm_64				*nm_64;

	// ft_printf("Handle 64 bits in %s endian\n", (endian) ? "little" : "big");
	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64(ptr_header, endian);
	sort_nm_64(nm_64, 0, endian);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print_64(nm_64, endian));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_32(void *ptr_header, int8_t endian)
{
	struct s_nm_32				*nm_32;

	// ft_printf("Handle 32 bits in %s endian\n", (endian) ? "little" : "big");
	// print_segments_32_deprecated(content);
	nm_32 = get_nm_32(ptr_header, endian);
	sort_nm_32(nm_32, 0, endian);
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

int8_t		match_and_use_magic_number(void *ptr_header, uint32_t magic_number, char *path)
{
	if (magic_number == MH_MAGIC_64)
		return (handle_64(ptr_header, 0));
	else if (magic_number == MH_CIGAM_64)
		return (handle_64(ptr_header, 1));
	else if (magic_number == MH_MAGIC)
		return (handle_32(ptr_header, 0));
	else if (magic_number == MH_CIGAM)
		return (handle_32(ptr_header, 1));
	ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
	return (-2);
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

int8_t		handle_fat_header_32(void *ptr_header, int8_t endian, char *path)
{
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_32;
	uint32_t			nb_arch;
	uint32_t			offset_arch = 0;
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
		ft_printf("\n%s (for architecture %s):\n", path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_32->cputype) : fat_arch_32->cputype));
		offset_arch = (endian) ? endian_swap_int32(fat_arch_32->offset) : fat_arch_32->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		// ft_printf(" └─> Magic_number [%X] for architecture n˚[%X]\n", magic_number, i);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, path);
		i++;
	}
	return (1);
}

int8_t		handle_fat_header_64(void *ptr_header, int8_t endian, char *path)
{
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_64;
	uint32_t			nb_arch;
	uint32_t			offset_arch = 0;
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
		ft_printf("\n%s (for architecture %s):\n", path, get_architecture_name((endian) ? endian_swap_int32(fat_arch_64->cputype) : fat_arch_64->cputype));
		offset_arch = (endian) ? endian_swap_int64(fat_arch_64->offset) : fat_arch_64->offset;
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		// ft_printf(" └─> Magic_number [%X] for architecture n˚[%X]\n", magic_number, i);
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, path);
		i++;
	}
	return (1);
}

int8_t		handle_ar(void *ptr_header, char *path)
{
	struct	ar_hdr	*ar_hdr;

	(void)path;
	(void)path;
	return (1);
}

int8_t		analyse_magic_number(void *ptr_header, char *path)
{
	uint32_t		magic_number;

	if (!ptr_header)
	{
		ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
		return (-1);
	}
	magic_number = *(uint32_t *)ptr_header;
	ft_printf("MAGIC NUMBER [%x] \n", *(uint64_t *)ptr_header);
	if (magic_number == FAT_MAGIC)
		return (handle_fat_header_32(ptr_header, 0, path));
	else if (magic_number == FAT_CIGAM)
		return (handle_fat_header_32(ptr_header, 1, path));
	else if (magic_number == AR_MAGIC)
		return (handle_ar(ptr_header, path));
	if (magic_number == FAT_MAGIC_64)
		return (handle_fat_header_64(ptr_header, 0, path));
	else if (magic_number == FAT_CIGAM_64)
		return (handle_fat_header_64(ptr_header, 1, path));
	return (match_and_use_magic_number(ptr_header, magic_number, path));
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
	if (analyse_magic_number((void*)content, path) == -1)
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
