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

CC		=	gcc

CLI_TOOLS	=	build dump-digests extract find

CLI_SRCS	=	$(addsuffix .c,$(CLI_TOOLS))

CLI_OBJS	=	$(CLI_SRCS:.c=.o)

CFLAGS		+=	-W -Wall -Wextra
CFLAGS		+=	-g

CFLAGS		+=	-I ./../btarr	\
			-I ../		\
			-I .

LDFLAGS		=	`pkg-config --libs openssl` -L . -lbtarr -lmerkle-tree -Wl,-rpath=.

RM		=	rm -rf

all		:	$(CLI_OBJS)
			make -C ../btarr
			make -C ../
			cp ../libbtarr.so .
			cp ../libmerkle-tree.so .
			for tool_name in $(CLI_TOOLS) ; do gcc -o merkle-tree-"$$tool_name" "$$tool_name".o $(LDFLAGS) ; done

clean		:
			$(RM) $(addprefix merkle-tree-,$(CLI_TOOLS))

fclean		:	clean
			$(RM) $(CLI_OBJS)
			$(RM) ./libbtarr.so
			$(RM) ./libmerkle-tree.so

re		:	fclean all

.PHONY		:	all clean fclean re
