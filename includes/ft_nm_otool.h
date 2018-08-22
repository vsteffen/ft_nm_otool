#ifndef ft_nm_otool_H
# define ft_nm_otool_H

# include "libft.h"
# include <sys/mman.h>
# include <sys/stat.h>
# include <mach-o/fat.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <mach-o/ranlib.h>
# include <ar.h>
# include <fcntl.h>

# define AR_MAGIC 0x72613c21

typedef struct			s_sym_32 {
	struct nlist		elem;
	char				*sym_table_string;
	size_t				order;
	struct s_sym_32		*next;
}						t_sym_32;

typedef struct			s_sect_32 {
	struct section		elem;
	struct s_sect_32	*next;
	size_t				ordinal;
}						t_sect_32;

typedef struct			s_nm_32 {
	struct s_sym_32		*sym_list;
	struct s_sect_32	*sect_list;
	size_t				sym_list_size;
}						t_nm_32;

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
}						t_sect_64;

typedef struct			s_nm_64 {
	struct s_sym_64		*sym_list;
	struct s_sect_64	*sect_list;
	size_t				sym_list_size;
}						t_nm_64;

typedef struct			s_file_st {
	char				*path;
	size_t				size;
	cpu_type_t			cputype;
}						t_file_st;

typedef struct			s_nm_data {
	char				*file_path;
	size_t				file_size;
	int8_t				*flag;
}						t_nm_data;

typedef struct			s_status {
	int8_t				res;
	int8_t				ret;
}						t_status;

void					print_sections_32(struct s_sect_32 *sect_list);
void					print_sections_32_deprecated(struct segment_command *seg);
void					print_segments_32_deprecated(char *content);
void					print_symboles_32(struct s_sym_32 *sym_list);
void					print_sections_64(struct s_sect_64 *sect_list);
void					print_sections_64_deprecated(struct segment_command_64 *seg);
void					print_segments_64_deprecated(char *content);
void					print_symboles_64(struct s_sym_64 *sym_list);

int8_t					iter_sym_table_and_print_32(struct s_nm_32 *nm_32, int8_t endian);
int8_t					iter_sym_table_and_print_64(struct s_nm_64 *nm_64, int8_t endian);

int32_t					endian_swap_int32(uint32_t x);
int64_t					endian_swap_int64(uint64_t x);

int8_t					get_location(int8_t n_type);
char					get_correct_letter(char letter, int8_t stab_ext_p_local);
struct s_sect_32		*get_section_32(struct s_sect_32 *sect_list, uint32_t ordinal);
struct s_sect_64		*get_section_64(struct s_sect_64 *sect_list, uint32_t ordinal);

struct s_nm_32			*get_nm_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data);
struct s_nm_64			*get_nm_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data);

void					sort_nm_32(struct s_nm_32 *nm_32, int8_t endian, struct s_nm_data *nm_data);
void					sort_nm_64(struct s_nm_64 *nm_64, int8_t endian, struct s_nm_data *nm_data);
int8_t					sort_alphabetically_64(struct s_sym_64 *elem1, \
	struct s_sym_64 *elem2, int8_t endian);
int8_t					sort_reverse_64(struct s_sym_64 *elem1, \
	struct s_sym_64 *elem2, int8_t endian);
int8_t					sort_alphabetically_32(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t endian);
int8_t					sort_reverse_32(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t endian);

int8_t					match_and_use_magic_number(void *ptr_header, uint32_t magic_number, struct s_nm_data *nm_data);
int8_t					find_segments_and_sections_32(void *ptr_header, int8_t endian, struct s_nm_data *nm_data);
int8_t					find_segments_and_sections_64(void *ptr_header, int8_t endian, struct s_nm_data *nm_data);

int8_t					exit_err(char *path, char *message);


#endif
