#ifndef WRITER_H
#define WRITER_H

#include <sys/types.h>
#include <elf.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int write_new_elf(const char *output_file, void *elf_map, size_t size); // void -> int yapıldı

#endif