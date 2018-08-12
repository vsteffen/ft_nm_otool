#include "ft_nm_otool.h"

int8_t		exit_nm(char *path, char *message)
{
	ft_putstr("ft_nm: ");
	ft_putstr(path);
	ft_putstr(message);
	return (EXIT_FAILURE);
}

int8_t		handle_32_big_endian(void *ptr_header)
{
	struct s_nm_32				*nm_32;

	ft_printf("Handle 32 bits big endian\n");
	// print_segments_32_deprecated(content);
	nm_32 = get_nm_32_big_endian(ptr_header);
	sort_nm_32(nm_32, 0);
	if (nm_32->sym_list)
		return (iter_sym_table_and_print_32(nm_32));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_64_big_endian(void *ptr_header)
{
	struct s_nm_64				*nm_64;

	ft_printf("Handle 64 bits big endian\n");
	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64_big_endian(ptr_header);
	sort_nm_64(nm_64, 0);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print_64(nm_64));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_32_little_endian(void *ptr_header)
{
	struct s_nm_32				*nm_32;

	ft_printf("Handle 32 bits little endian\n");
	// print_segments_32_deprecated(content);
	nm_32 = get_nm_32_little_endian(ptr_header);
	sort_nm_32(nm_32, 0);
	if (nm_32->sym_list)
		return (iter_sym_table_and_print_32(nm_32));
	ft_putstr("ft_nm: The file was not recognized as a valid object file\n");
	return (-1);
}

int8_t		handle_64_little_endian(void *ptr_header)
{
	struct s_nm_64				*nm_64;

	ft_printf("Handle 64 bits little endian\n");
	// print_segments_64_deprecated(content);
	nm_64 = get_nm_64_little_endian(ptr_header);
	sort_nm_64(nm_64, 0);
	if (nm_64->sym_list)
		return (iter_sym_table_and_print_64(nm_64));
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
		return (handle_64_big_endian(ptr_header));
	else if (magic_number == MH_CIGAM_64)
		return (handle_64_little_endian(ptr_header));
	else if (magic_number == MH_MAGIC)
		return (handle_32_big_endian(ptr_header));
	else if (magic_number == MH_CIGAM)
		return (handle_32_little_endian(ptr_header));
	ft_printf("ft_nm: %s: The file was not recognized as a valid object file\n", path);
	return (-2);
}

int8_t		handle_fat_header_32(void *ptr_header, int8_t endian, char *path)
{
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_32;
	uint32_t			nb_arch;
	uint32_t			offset_arch = 0;
	uint32_t		magic_number;
	size_t				i;

	fat_header = (struct fat_header*)ptr_header;
	fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header));
	if (endian == 1)
		nb_arch = endian_swap_int32(fat_header->nfat_arch);
	else
		nb_arch = fat_header->nfat_arch;
	i = 0;
	ft_printf("ft_nm: fat_header 32 bits [%X] with [%zu] architectures\n", (endian == 0) ? FAT_MAGIC : FAT_CIGAM, nb_arch);
	while (i < nb_arch)
	{
		fat_arch_32 = (struct fat_arch*)(ptr_header + sizeof(struct fat_header) + sizeof(struct fat_arch) * i);
		// ft_printf("Offset [%x]\n", fat_arch_32->offset);
		if (endian == 1)
			offset_arch = endian_swap_int32(fat_arch_32->offset);
		else
			offset_arch = fat_arch_32->offset;
		// offset_arch = ft_hexstr_to_int64_t((const unsigned char *)ft_int64_to_array(offset_arch));
		// ft_printf("New Offset [%x]\n", offset_arch);
		magic_number = *(uint32_t *)(ptr_header + offset_arch);
		// ft_printf("fat_arch->offset [%x] - offset_arch [%x]\n", fat_arch_32->offset, offset_arch);
		ft_printf(" └─> Magic_number [%X] for architecture n˚[%X]\n", magic_number, i);
		// ft_printf("ptr_header [%p] and struct address [%p]\n", ptr_header, ptr_header + fat_arch_32->offset);
		// fat_arch_32 = (struct fat_arch*)((void*)fat_arch_32 + sizeof(struct fat_arch));
		match_and_use_magic_number(ptr_header + offset_arch, magic_number, path);
		ft_putchar('\n');
		i++;
	}
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
	if (magic_number == FAT_MAGIC)
		return (handle_fat_header_32(ptr_header, 0, path));
	else if (magic_number == FAT_CIGAM)
		return (handle_fat_header_32(ptr_header, 1, path));
	else if (magic_number == FAT_MAGIC_64 || magic_number == FAT_CIGAM_64)
	{
		ft_printf("ft_nm: %s: There is an fat 64 header ... a big one\n", path);
		return (1);
	}
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
