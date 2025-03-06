CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iheaders
SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c
OBJ = $(SRC:.c=.o)
OUT = woody_woodpacker

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(OUT)

re: fclean all
