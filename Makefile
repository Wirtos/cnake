CC = cc
SHELL = /bin/sh
CFLAGS += -Iinclude -Wall -Werror -Wextra $(shell pkg-config --libs ncurses)
PREFIX = /usr/local

NAME = snake

SRCS:= $(shell find -name "*.c")

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

.o: .c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -delete -print

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

install: $(NAME)
	cp $(NAME) $(PREFIX)/bin
	chmod +x $(PREFIX)/bin/$(NAME)

uninstall:
	rm $(PREFIX)/bin/$(NAME)

debug: CFLAGS += -g
debug: fclean $(NAME)

.PHONY: all clean fclean re install uninstall debug
