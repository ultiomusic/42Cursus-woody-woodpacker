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
    Elf64_Phdr *text_seg = find_text_segment(elf_map);
    if (!text_seg) {
        fprintf(stderr, "No .text segment found\n");
        return -1;
    }

    if (mprotect((char *)elf_map + text_seg->p_offset, text_seg->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        perror("mprotect failed");
        return -1;
    }

    // `/dev/urandom` üzerinden rastgele anahtar oku
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/urandom failed");
        return -1;
    }

    unsigned char key[KEY_SIZE];
    if (read(fd, key, KEY_SIZE) != KEY_SIZE) {
        perror("read /dev/urandom failed");
        close(fd);
        return -1;
    }
    close(fd);

    // Anahtarı HEX formatına çevir (07A51FF040D45D5CD gibi)
    char key_str[KEY_SIZE * 2 + 1];  // 18 karakter + NULL
    for (int i = 0; i < KEY_SIZE; i++) {
        sprintf(&key_str[i * 2], "%02X", key[i]);
    }
    key_str[KEY_SIZE * 2] = '\0';  // NULL-terminate

    // Anahtarı ekrana yazdır
    printf("Encryption key: %s\n", key_str);;

    // Şifreleme işlemini uygula
    xor_encrypt((char *)elf_map + text_seg->p_offset, text_seg->p_memsz, key, KEY_SIZE);

    // Anahtarı ELF dosyasının bir yerine göm
    unsigned char *key_storage = (unsigned char *)elf_map + text_seg->p_offset - KEY_SIZE;
    memcpy(key_storage, key, KEY_SIZE);

    return 0; // Başarılı
}