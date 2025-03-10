#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>

void *map_elf(const char *filename, size_t *size);
Elf64_Phdr *find_text_segment(void *elf_map);

#endif

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include "elf_utils.h"

void xor_encrypt(void *data, size_t size, unsigned char key);
void encrypt_text_section(void *elf_map);

#endif

#ifndef WRITER_H
#define WRITER_H

#include "elf_utils.h"

void write_new_elf(const char *output_file, void *elf_map, size_t size);

#endif

.global _start
.section .text
_start:
%ifdef __x86_64__  

    mov rdi, message
    call puts

    mov rsi, key
    mov rdi, text_start
    mov rcx, text_size

decrypt_loop:
    xor byte ptr [rdi], rsi
    inc rdi
    loop decrypt_loop

    jmp text_start

%elif __i386__  

    push message
    call puts
    add esp, 4  

    mov esi, text_start   
    mov ecx, text_size    
    mov al, key           

decrypt_loop:
    xor byte ptr [esi], al 
    inc esi                
    loop decrypt_loop      

    jmp text_start         

%endif

.section .data
message:
    .asciz "....WOODY...."

key:
    .byte 0xAA 

.section .bss
.lcomm text_start, 100
.lcomm text_size, 4


#include "elf_utils.h"

void *map_elf(const char *filename, size_t *size) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return NULL;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return NULL;
    }

    *size = st.st_size;
    void *map = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    if (map == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return map;
}

Elf64_Phdr *find_text_segment(void *elf_map) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_map;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)elf_map + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X)) {
            return &phdr[i]; 
        }
    }
    return NULL;
}


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

    // Bellek izinlerini değiştirme
    if (mprotect((void *)((uintptr_t)text_addr & ~0xFFF), text_seg->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
        perror("mprotect failed");
        return;
    }

    printf("Encrypting .text section...\n");
    xor_encrypt(text_addr, text_seg->p_memsz, 0xAA);
}


#include "writer.h"

void write_new_elf(const char *output_file, void *elf_map, size_t size) {
    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        perror("open");
        return;
    }

    if (write(fd, elf_map, size) != (ssize_t)size) {
        perror("write");
    }

    close(fd);
}

#include "elf_utils.h"
#include "encryption.h"
#include "writer.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    size_t elf_size;
    void *elf_map = map_elf(argv[1], &elf_size);
    if (!elf_map) return 1;

    encrypt_text_section(elf_map);
    write_new_elf("woody", elf_map, elf_size);

    munmap(elf_map, elf_size);
    printf("Encrypted ELF saved as woody\n");
    return 0;
}

