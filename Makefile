# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vsteffen <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/06 20:52:05 by vsteffen          #+#    #+#              #
#    Updated: 2018/03/20 18:17:27 by vsteffen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_nm_otool

CC 		=	/usr/bin/clang
RM 		=	/bin/rm
MAKE 	=	/usr/bin/make -C
MKDIR 	=	/bin/mkdir -p
AR 		=	/usr/bin/ar
RANLIB 	=	/usr/bin/ranlib
GIT		=	/usr/bin/git

OBJ = $(patsubst %.c, $(OPATH)/%.o, $(SRC))

CFLAGS = -Wall -Wextra -Werror -g

LIB		=	$(ROOT)/lib
LIBSRCS	=	$(ROOT)/libsrcs
LIBFT 	=	$(LIBSRCS)/libft

ROOT  	=	$(shell /bin/pwd)
OPATH 	=	$(ROOT)/objs
CPATH 	=	$(ROOT)/srcs
LPATH	=	$(LIBFT)/libft.a
HPATH 	=	-I $(ROOT)/includes -I $(LIBFT)/includes

SRC =	ft_nm_otool.c

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

$(NAME): $(OPATH) $(OBJ)
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Make\"\033[0m"
	@echo "Building $@"
	@$(CC) -o $@ $(CFLAGS) $(OBJ) $(LPATH) $(HPATH)
	@echo "\033[32m ╔════════════════╗"
	@echo " ║  All is done ! ║"
	@echo " ╚════════════════╝\033[0m"

$(OPATH)/%.o: $(CPATH)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@ $(HPATH)

$(OPATH):
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Building objs\"\033[0m"
	@echo "Creating OBJ directory"
	@$(MKDIR) $@
	@echo "Creating OBJ files if they do not exist or have changed"

clean:
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Clean\"\033[0m"
	@echo "Deleting OBJS."
	$(RM) -Rf $(OPATH)
	@echo "\033[32mOBJS deleted.\033[0m\n"

fclean: clean
	@echo "\n\033[33m\033[4m\033[1m → ft_nm_otool \"Fclean\"\033[0m"
	@echo "Deleting $(NAME)."
	$(RM) -f $(NAME)
	@echo "\033[32m$(NAME) deleted.\033[0m\n"
	@echo "Deleting lib content."
	@$(MAKE) $(LIBFT) fclean
	@echo "\033[32mLib content deleted.\033[0m\n"


re: fclean all
