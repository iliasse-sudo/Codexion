NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

INCLUDES = ./includes

SRCS = src/codexion.c \
		src/atoll.c \
		src/parser.c \
		src/heap.c \
		src/heap_ops.c \
		src/allocs.c \
		src/frees.c \
		src/inits.c

OBJS = $(SRCS:.c=.o)
	
HDRS = $(INCLUDES)/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
