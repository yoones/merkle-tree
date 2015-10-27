##
## This file is part of libmerkle-tree.
##
## libmerkle-tree is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## libmerkle-tree is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with libmerkle-tree.  If not, see <http://www.gnu.org/licenses/>.
##

CC		=	gcc -fPIC -shared

LIB_NAME	=	libmerkle-tree.so

LIB_SRCS	=	mt_build.c	\
			mt_extract.c	\
			mt_find.c	\
			mt_helpers.c	\
			mt_load.c

LIB_OBJS	=	$(LIB_SRCS:.c=.o)

CFLAGS		+=	-W -Wall -Wextra
CFLAGS		+=	-g
CFLAGS		+=	-I ./btarr -I .

LDFLAGS		=	`pkg-config --libs openssl` -L btarr -L . -lbtarr

RM		=	rm -rf

all		:	$(LIB_NAME)

$(LIB_NAME)	:	$(LIB_OBJS)
			make -C ./btarr
			cp ./btarr/libbtarr.so .
			$(CC) -o $(LIB_NAME) $(LIB_OBJS)
			make -C ./cli

clean		:
			$(RM) $(LIB_NAME)
			make -C btarr clean
			make -C cli clean

fclean		:	clean
			$(RM) $(LIB_OBJS)
			make -C btarr fclean
			$(RM) ./libbtarr.so
			make -C cli fclean

re		:	fclean all

.PHONY		:	all clean fclean re
