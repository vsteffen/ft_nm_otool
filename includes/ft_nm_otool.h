#ifndef ft_nm_otool_H
# define ft_nm_otool_H

# include "libft.h"
# include <sys/mman.h>
# include <sys/stat.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <fcntl.h>

typedef struct			s_sym_64 {
	struct nlist_64		elem;
	char				*sym_table_string;
	size_t				order;
	struct s_sym_64		*next;
}						t_sym_64;

typedef struct			s_sect_64 {
	struct section_64	elem;
	struct s_sect_64	*next;
	size_t				ordinal;
}						t_sect;

typedef struct			s_nm_64 {
	struct s_sym_64		*sym_list;
	struct s_sect_64	*sect_list;
	size_t				sym_list_size;
}						t_nm_data;

struct s_nm_64		*get_nm_64(char *content);

void		print_sections_64(struct s_sect_64 *sect_list);
void		print_sections_64_deprecated(struct segment_command_64 *seg);
void		print_segments_64_deprecated(char *content);
void		print_symboles_64(struct s_sym_64 *sym_list);

// void		sort_nm_64(struct s_nm_64 *nm_64);
void		sort_nm_64(struct s_nm_64 *nm_64, int8_t sort_selected);

#endif
