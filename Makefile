CC=clang
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic
LDFLAGS=

SRC=main.c csv.c avl_usines.c network.c histo.c leaks.c
OBJ=$(SRC:.c=.o)

.PHONY: all clean

all: wildwater

wildwater: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) wildwater
