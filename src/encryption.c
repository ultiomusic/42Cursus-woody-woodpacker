#include "encryption.h"

void xor_encrypt(void *data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        ((unsigned char*)data)[i] ^= key;
    }
}

void encrypt_text_section(void *elf_map) {
    Elf64_Phdr *text_seg = find_text_segment(elf_map);
    if (!text_seg) {
        printf("No .text segment found\n");
        return;
    }

    printf("Encrypting .text section...\n");
    xor_encrypt((char *)elf_map + text_seg->p_offset, text_seg->p_filesz, 0xAA);
}