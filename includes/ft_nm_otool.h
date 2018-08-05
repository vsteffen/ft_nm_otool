#ifndef ft_nm_otool_H
# define ft_nm_otool_H

# include "libft.h"
# include <sys/mman.h>
# include <sys/stat.h>
# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <fcntl.h>

typedef struct		s_elem {
	struct s_elem	*prec;
	struct s_elem	*next;
}					t_elem;


#endif
