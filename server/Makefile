# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/05/11 15:56:03 by obamzuro          #+#    #+#              #
#    Updated: 2018/10/14 23:38:24 by obamzuro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = server

SRCNAME = 	server.c\
			handle_client.c\
			response_compress.c\
			response_other.c\
			additional_funcs.c

FLAGS = -g

SRC = $(addprefix src/, $(SRCNAME))

OBJ = $(SRC:.c=.o)

LIBDIR = ../libft

all: lib $(NAME)

lib:
	make -C ../libft

$(NAME): $(OBJ) $(LIBDIR)/libft.a
	gcc $(FLAGS) -o $(NAME) $(OBJ) -I $(LIBDIR)/include -I inc/ -L $(LIBDIR) -lft

%.o: %.c inc/server.h
	gcc $(FLAGS) -c $< -o $@ -I $(LIBDIR)/include -I inc/

clean:
	find . -name "*.o" -exec rm -rf {} \;
	find . -name ".*.sw[pon]" -exec rm -rf {} \;
	make -C $(LIBDIR) clean

fclean: clean
	rm -rf $(NAME)
	make -C $(LIBDIR) fclean

re: fclean all
