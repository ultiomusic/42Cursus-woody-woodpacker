#ifndef WRITER_H
#define WRITER_H

#include "elf_utils.h"

void write_new_elf(const char *output_file, void *elf_map, size_t size);

#endif
