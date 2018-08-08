#ifndef ft_nm_otool_H
# define ft_nm_otool_H

# include "libft.h"
# include <sys/mman.h>
# include <sys/stat.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <fcntl.h>

typedef struct		s_symbol_table {
	struct s_symbol	*current;
	struct s_symbol	*next;
}					t_symbol_table;


#endif
