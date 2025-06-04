#include "encryption.h"
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

void xor_encrypt(void *data, size_t size, unsigned char *key, size_t key_size) {
    for (size_t i = 0; i < size; i++) {
        ((unsigned char*)data)[i] ^= key[i % key_size];
    }
}

int encrypt_text_section(void *elf_map) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_map;
    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)elf_map + ehdr->e_shoff);
    const char *shstrtab = (char *)elf_map + shdr[ehdr->e_shstrndx].sh_offset;
    Elf64_Off text_offset = 0;
    Elf64_Xword text_size = 0;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(shstrtab + shdr[i].sh_name, ".text") == 0) {
            text_offset = shdr[i].sh_offset;
            text_size = shdr[i].sh_size;
            break;
        }
    }
    if (!text_offset || !text_size) {
        fprintf(stderr, "No .text section found\n");
        return -1;
    }

    unsigned char key[KEY_SIZE];
    for (int i = 0; i < KEY_SIZE; i++)
        key[i] = 42;

    char key_str[KEY_SIZE * 2 + 1];
    for (int i = 0; i < KEY_SIZE; i++) {
        sprintf(&key_str[i * 2], "%02X", key[i]);
    }
    key_str[KEY_SIZE * 2] = '\0';  // NULL-terminate

    printf("Encryption key: %s\n", key_str);

    xor_encrypt((char *)elf_map + text_offset, text_size, key, KEY_SIZE);

    // Optionally store the key somewhere if needed

    return 0; // Başarılı
}