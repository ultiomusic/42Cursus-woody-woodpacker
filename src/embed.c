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
    //printf("[embed_payload] p_offset=0x%lx, p_filesz(before)=0x%lx, new_offset=0x%lx, payload_size=0x%lx\n",
    //    last_load->p_offset, last_load->p_filesz, new_offset, payload_size);
    memcpy((char *)map + new_offset, payload, payload_size);
    last_load->p_filesz += payload_size;
    last_load->p_memsz += payload_size;
    last_load->p_flags |= PF_X | PF_W | PF_R; 
    ehdr->e_entry = last_load->p_vaddr + last_load->p_memsz - payload_size;
}