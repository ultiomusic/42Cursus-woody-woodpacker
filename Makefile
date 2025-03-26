CC = gcc
ASM = nasm
CFLAGS = -Wall -Wextra -Werror -Iinc -m64
ASMFLAGS = -f elf64

SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c
ASMSRC = src/decryptor.s
OBJ = $(SRC:.c=.o) $(ASMSRC:.s=.o)
OUT = woody_woodpacker

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(OUT)

re: fclean all

.PHONY: all clean fclean re