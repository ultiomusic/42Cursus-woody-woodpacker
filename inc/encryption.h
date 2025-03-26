#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "elf_utils.h"

void xor_encrypt(void *data, size_t size, unsigned char key);
int encrypt_text_section(void *elf_map); // void -> int yapıldı

#endif