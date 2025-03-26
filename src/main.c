#include "elf_utils.h"
#include "encryption.h"
#include "writer.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    size_t elf_size;
    void *elf_map = map_and_check_elf(argv[1], &elf_size);
    if (!elf_map) {
        return 1; // Hata mesajı zaten verildi
    }

    encrypt_text_section(elf_map);
    write_new_elf("woody", elf_map, elf_size);

    if (munmap(elf_map, elf_size) == -1) {
        perror("munmap failed");
        return 1;
    }

    printf("Encrypted ELF saved as woody\n");
    return 0;
}