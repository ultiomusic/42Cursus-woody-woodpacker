CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinc
SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c
OBJ = $(SRC:.c=.o)
OUT = woody_woodpacker

# Mimari Seçenekleri
ARCH ?= 64  # Varsayılan olarak 64-bit derler
ifeq ($(ARCH),32)
    CFLAGS += -m32
    OUT := woody_woodpacker_32
else
    CFLAGS += -m64
    OUT := woody_woodpacker
endif

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f woody_woodpacker_32 woody_woodpacker

re: fclean all

# Debug Modu (GDB için)
debug: CFLAGS += -g
debug: re

# Release Modu (Optimizasyonlu)
release: CFLAGS += -O2
release: re
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinc
SRC = src/main.c src/elf_utils.c src/encryption.c src/writer.c
OBJ = $(SRC:.c=.o)
OUT = woody_woodpacker

# Mimari Seçenekleri
ARCH ?= 64  # Varsayılan olarak 64-bit derler
ifeq ($(ARCH),32)
    CFLAGS += -m32
    OUT := woody_woodpacker_32
else
    CFLAGS += -m64
    OUT := woody_woodpacker
endif

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f woody_woodpacker_32 woody_woodpacker

re: fclean all

# Debug Modu (GDB için)
debug: CFLAGS += -g
debug: re

# Release Modu (Optimizasyonlu)
release: CFLAGS += -O2
release: re
