/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm_otool.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/30 19:19:42 by vsteffen          #+#    #+#             */
/*   Updated: 2018/08/30 19:19:43 by vsteffen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_OTOOL_H
# define FT_NM_OTOOL_H

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
	size_t				ti;
}						t_nm_data;

typedef struct			s_status {
	int8_t				res;
	int8_t				ret;
}						t_status;

typedef struct			s_ar {
	uint64_t			ran_off;
	int8_t				is_32_ar;
	size_t				i;
	size_t				j;
	size_t				symtab_pos;
	uint64_t			offset_file_tmp;
}						t_ar;

typedef struct			s_fat {
	struct fat_header	*fat_header;
	struct fat_arch		*fat_arch_32;
	struct fat_arch_64	*fat_arch_64;
	uint32_t			nb_arch;
	uint32_t			offset_arch;
	uint32_t			magic_number;
	size_t				i;
}						t_fat;

typedef struct symtab_command		t_symtab_cmd;
typedef struct segment_command		t_seg_cmd_32;
typedef struct mach_header			t_mach_header_32;

typedef struct			s_get_nm_32 {
	size_t				i;
	size_t				count_sect;
	struct s_sect_32	*sect_last_list;
	struct mach_header	*header;
	struct load_command	*lc;
	struct s_nm_32		*nm_32;
	t_symtab_cmd		*sym_command;
	t_seg_cmd_32		*seg;
	uint32_t			header_ncmds;
	uint32_t			lc_cmd;
}						t_get_nm_32;

typedef struct segment_command_64	t_seg_cmd_64;
typedef struct mach_header_64		t_mach_header_64;

typedef struct			s_get_nm_64 {
	size_t				i;
	size_t				count_sect;
	struct s_sect_64	*sect_last_list;
	t_mach_header_64	*header;
	struct load_command	*lc;
	struct s_nm_64		*nm_64;
	t_symtab_cmd		*sym_command;
	t_seg_cmd_64		*seg;
	uint32_t			header_ncmds;
	uint32_t			lc_cmd;
}						t_get_nm_64;

typedef struct			s_sect_tmp {
	size_t				i;
	uint64_t			header_ncmds;
	uint64_t			lc_cmd;

}						t_sect_tmp;

int8_t					get_flag_nm(int8_t flag[2], int ac, char **av);
int8_t					get_flag_otool(int8_t flag[2], int ac, char **av);
int8_t					verif_files_and_print_filename_if_one_file(int ac, \
	char **av);
int8_t					is_flag_nm(char *arg);
int8_t					is_flag_otool(char *arg);
int8_t					verif_args_files_and_get_nb_files(int ac, char **av);

int8_t					analyse_magic_number(void *ptr_header, \
	struct s_nm_data *nm_data);

int8_t					handle_fat_header_32(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data);
int8_t					handle_fat_header_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data);

int8_t					handle_ar(void *ptr_header, struct s_nm_data *nm_data);
void					find_next_ar_header(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data);
void					*find_begin_ar_file(void *ptr_header, size_t *i, \
	struct s_nm_data *nm_data);

int8_t					iter_sym_table_and_print_32(struct s_nm_32 *nm_32, \
	int8_t endian);
int8_t					iter_sym_table_and_print_64(struct s_nm_64 *nm_64, \
	int8_t endian);

int32_t					endian_swap_int32(uint32_t x);
int64_t					endian_swap_int64(uint64_t x);

int8_t					get_location(int8_t n_type);
char					get_correct_letter(char letter, \
	int8_t stab_ext_p_local);
struct s_sect_32		*get_section_32(struct s_sect_32 *sect_list, \
	uint32_t ordinal);
struct s_sect_64		*get_section_64(struct s_sect_64 *sect_list, \
	uint32_t ordinal);

struct s_nm_32			*get_nm_32(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data);
struct s_nm_64			*get_nm_64(void *ptr_header, int8_t endian, \
	struct s_nm_data *nm_data);
void					free_nm_32(struct s_nm_32 *nm_32);
void					free_nm_64(struct s_nm_64 *nm_64);

void					sort_nm_32(struct s_nm_32 *nm_32, int8_t endian, \
	struct s_nm_data *nm_data);
void					sort_nm_64(struct s_nm_64 *nm_64, int8_t endian, \
	struct s_nm_data *nm_data);
int8_t					sort_alphabetically_64(struct s_sym_64 *elem1, \
	struct s_sym_64 *elem2, int8_t endian);
int8_t					sort_reverse_64(struct s_sym_64 *elem1, \
	struct s_sym_64 *elem2, int8_t endian);
int8_t					sort_alphabetically_32(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t endian);
int8_t					sort_reverse_32(struct s_sym_32 *elem1, \
	struct s_sym_32 *elem2, int8_t endian);

int8_t					match_and_use_magic_number(void *ptr_header, \
	uint32_t magic_number, struct s_nm_data *nm_data);
int8_t					find_segments_and_sections_32(void *ptr_header, \
	int8_t endian, struct s_nm_data *nm_data);
int8_t					find_segments_and_sections_64(void *ptr_header, \
	int8_t endian, struct s_nm_data *nm_data);
int8_t					is_ppc_arch_32(void *ptr_header, int8_t endian);
void					hexdump_ppc_32_print(void *content, size_t i, \
	uint32_t addr);
int8_t					hexdump_ppc_32(void *ptr_header, struct section *sect, \
	int8_t endian, struct s_nm_data *nm_data);
int8_t					is_ppc_arch_64(void *ptr_header, int8_t endian);
void					hexdump_ppc_64_print(void *content, size_t i, \
	uint64_t addr);
int8_t					hexdump_ppc_64(void *ptr_header, \
	struct section_64 *sect, int8_t endian, struct s_nm_data *nm_data);

int8_t					exit_err(char *path, char *message);

#endif
