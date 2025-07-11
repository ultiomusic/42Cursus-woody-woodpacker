CC = gcc
ASM = nasm
CFLAGS = -Wall -Wextra -Werror -Iinc -m64
ASMFLAGS = -f elf64

SRC = src/main.c src/encryption.c src/writer.c src/embed.c src/payload_patch.c
OBJ = $(SRC:.c=.o)
OUT = woody_woodpacker

all: $(INC_PAYLOAD) $(OUT)
INC_PAYLOAD = src/woody_payload.inc
XXD = xxd

src/woody_payload.o: src/woody_payload.s
	$(ASM) $(ASMFLAGS) -o $@ $<

src/woody_payload.bin: src/woody_payload.o
	objcopy -O binary --only-section=.text src/woody_payload.o src/woody_payload.bin

$(XXD): src/xxd.c
	$(CC) $(CFLAGS) -o $@ $<

src/woody_payload.inc: src/woody_payload.bin $(XXD)
	./$(XXD) src/woody_payload.bin > src/woody_payload.inc

src/main.o: src/woody_payload.inc


$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f $(OBJ) src/woody_payload.bin src/woody_payload.inc woody woody_woodpacker $(XXD)

fclean: clean
	rm -f $(OUT) src/woody_payload.o

re: fclean all

.PHONY: all clean fclean re