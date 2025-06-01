#include "elf_utils.h"
#include "encryption.h"
#include "writer.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    const char *self_name = "woody_woodpacker";
    size_t self_len = strlen(self_name);
    size_t input_len = strlen(argv[1]);

    if (input_len >= self_len && 
        strcmp(argv[1] + input_len - self_len, self_name) == 0) {
        fprintf(stderr, "Error: Cannot pack the packer itself\n");
        return 1;
    }

    size_t elf_size;
    void *elf_map = map_and_check_elf(argv[1], &elf_size);
    if (!elf_map) return 1;

    if (encrypt_text_section(elf_map) != 0) {
        munmap(elf_map, elf_size);
        return 1;
    }

    if (write_new_elf("woody", elf_map, elf_size) != 0) {
        munmap(elf_map, elf_size);
        return 1;
    }

    munmap(elf_map, elf_size);
    printf("Success: Encrypted ELF saved as woody\n");
    return 0;
}