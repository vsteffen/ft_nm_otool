# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/06 20:52:05 by vsteffen          #+#    #+#              #
#    Updated: 2018/08/07 17:58:24 by vsteffen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ft_nm_otool

CC 		=	/usr/bin/clang
RM 		=	/bin/rm
MAKE 	=	/usr/bin/make -C
MKDIR 	=	/bin/mkdir -p
AR 		=	/usr/bin/ar
RANLIB 	=	/usr/bin/ranlib
GIT		=	/usr/bin/git

CFLAGS	=	-Wall -Wextra -Werror -g

LIB		=	$(ROOT)/lib
LIBSRCS	=	$(ROOT)/libsrcs
LIBFT 	=	$(LIBSRCS)/libft

ROOT  	=	$(shell /bin/pwd)
OPATH 	=	$(ROOT)/objs
CPATH 	=	$(ROOT)/srcs
LPATH	=	$(LIBFT)/libft.a
HPATH 	=	-I $(ROOT)/includes -I $(LIBFT)/includes

NM 		= 	ft_nm
NM_SRC	=	$(NM)/ft_nm.c \
			$(NM)/print_nm.c \
			$(NM)/get_nm_64.c \
			$(NM)/sort.c

NM_OBJ	=	$(patsubst %.c, $(OPATH)/%.o, $(NM_SRC))

OTOOL 		= 	ft_otool
OTOOL_SRC	=	$(OTOOL)/ft_otool.c

OTOOL_OBJ	=	$(patsubst %.c, $(OPATH)/%.o, $(OTOOL_SRC))

.PHONY: all clean fclean re

all: pre-check-submodule pre-check-lib $(NAME)

pre-check-submodule:
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Pre check submodule\"\033[0m"
	@echo "Update submodules"
	@$(GIT) submodule init
	@$(GIT) submodule update --recursive --remote

pre-check-lib:
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Pre check lib\"\033[0m"
	@echo "Compile or verify lib"
	@$(MAKE) $(LIBFT)

$(NAME): $(OPATH) $(OTOOL_OBJ) $(NM_OBJ)
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Make\"\033[0m"
	@echo "Building $(NM)"
	@$(CC) -o $(NM) $(CFLAGS) $(NM_OBJ) $(LPATH) $(HPATH)
	@echo "Building $(OTOOL)"
	@$(CC) -o $(OTOOL) $(CFLAGS) $(OTOOL_OBJ) $(LPATH) $(HPATH)
	@echo "\033[32m ╔════════════════╗"
	@echo " ║  All is done ! ║"
	@echo " ╚════════════════╝\033[0m"

$(OPATH)/%.o: $(CPATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(HPATH)

$(OPATH):
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Building objs\"\033[0m"
	@echo "Creating OBJ directory"
	@$(MKDIR) $@ $@/$(NM) $@/$(OTOOL)

clean:
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Clean\"\033[0m"
	@echo "Deleting OBJS."
	$(RM) -Rf $(OPATH)
	@echo "\033[32mOBJS deleted.\033[0m\n"

fclean: clean
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Fclean\"\033[0m"
	@echo "Deleting $(NM)."
	$(RM) -f $(NM)
	@echo "Deleting $(OTOOL)."
	$(RM) -f $(OTOOL)
	@echo "\033[32m$(NM) and $(OTOOL) deleted.\033[0m\n"

libft:
	@$(MAKE) $(LIBFT) fclean
	@$(MAKE) $(LIBFT)
	@echo "\033[32mLib content recompiled.\033[0m\n"

re: fclean all
