#include "encryption.h"
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

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

    void *text_addr = (char *)elf_map + text_seg->p_offset;

    // Bellek korumasını değiştirme
    if (mprotect(text_addr, text_seg->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
    {
        perror("mprotect failed");
        return;
    }

    printf("Encrypting .text section...\n");
    xor_encrypt(text_addr, text_seg->p_memsz, 0xAA);
}
