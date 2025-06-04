CC = gcc
ASM = nasm
CFLAGS = -Wall -Wextra -Werror -Iinc -m64
ASMFLAGS = -f elf64

SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c src/embed.c src/payload_patch.c
ASMSRC = src/decryptor.s #src/encrypt.s
OBJ = $(SRC:.c=.o) $(ASMSRC:.s=.o)
OUT = woody_woodpacker

all: $(INC_PAYLOAD) $(OUT)
INC_PAYLOAD = src/woody_payload.inc

# src/woody_payload.o: src/woody_payload.c
# 	$(CC) $(CFLAGS) -c -o $@ $<

src/woody_payload.o: src/woody_payload.s
	$(ASM) $(ASMFLAGS) -o $@ $<

src/woody_payload.bin: src/woody_payload.o
	objcopy -O binary --only-section=.text src/woody_payload.o src/woody_payload.bin

src/woody_payload.inc: src/woody_payload.bin
	xxd -i src/woody_payload.bin > src/woody_payload.inc

src/main.o: src/woody_payload.inc


$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f $(OBJ) src/woody_payload.bin src/woody_payload.inc woody woody_woodpacker

fclean: clean
	rm -f $(OUT)

re: fclean all

.PHONY: all clean fclean re