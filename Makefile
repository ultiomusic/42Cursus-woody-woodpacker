CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinc -m64

SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c
OBJ = $(SRC:.c=.o) $(ASMSRC:.s=.o)
OUT = woody_woodpacker

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(OUT)

re: fclean all

.PHONY: all clean fclean re