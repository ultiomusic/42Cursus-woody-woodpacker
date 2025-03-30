#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#define KEY_SIZE 9

#include "elf_utils.h"
#include <string.h>

void xor_encrypt(void *data, size_t size, unsigned char *key, size_t key_size);
int encrypt_text_section(void *elf_map);

#endif