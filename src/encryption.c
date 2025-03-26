#include "encryption.h"
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

void xor_encrypt(void *data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        ((unsigned char*)data)[i] ^= key;
    }
}

int encrypt_text_section(void *elf_map) {
    Elf64_Phdr *text_seg = find_text_segment(elf_map);
    if (!text_seg) {
        fprintf(stderr, "No .text segment found\n");
        return -1;
    }

    if (mprotect((char *)elf_map + text_seg->p_offset, text_seg->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        perror("mprotect failed");
        return -1;
    }

    xor_encrypt((char *)elf_map + text_seg->p_offset, text_seg->p_memsz, 0xAA);
    return 0; // Başarılı
}