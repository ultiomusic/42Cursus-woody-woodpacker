#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>

void *map_and_check_elf(const char *filename, size_t *size);
Elf64_Phdr *find_text_segment(void *elf_map);

#endif