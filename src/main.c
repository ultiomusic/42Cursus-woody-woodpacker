#define _GNU_SOURCE
#include "elf_utils.h"
#include "encryption.h"
#include "writer.h"
#include "embed.h"
#include "payload_patch.h"
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>

#include "woody_payload.h"
#include "woody_payload.inc"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF binary>\n", argv[0]);
        return 1;
    }

    const char *self_name = "woody_woodpacker";
    size_t self_len = strlen(self_name);
    size_t input_len = strlen(argv[1]);

    if (input_len >= self_len && 
        strcmp(argv[1] + input_len - self_len, self_name) == 0) {
        fprintf(stderr, "Error: Cannot pack the packer itself\n");
        return 1;
    }

    size_t elf_size;
    void *elf_map = map_and_check_elf(argv[1], &elf_size);
    if (!elf_map) return 1;

    Elf64_Phdr *text_seg = find_text_segment(elf_map);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_map;
    Elf64_Addr orig_entry = ehdr->e_entry;
    unsigned char patched_payload[src_woody_payload_bin_len];
    memcpy(patched_payload, src_woody_payload_bin, src_woody_payload_bin_len);
    printf("patch: text_vaddr=0x%lx text_size=0x%lx orig_entry=0x%lx\n",
        text_seg->p_vaddr, text_seg->p_memsz, orig_entry);
    patch_payload(patched_payload, src_woody_payload_bin_len, text_seg->p_vaddr, text_seg->p_memsz, orig_entry);
    printf("patched_payload first 16 bytes: ");
    for (unsigned int i = 0; i < 16 && i < src_woody_payload_bin_len; i++)
        printf("%02x ", patched_payload[i]);
    printf("\n");

    size_t new_size = elf_size + src_woody_payload_bin_len;
    void *output_elf = calloc(1, new_size); 
    if (!output_elf) {
        fprintf(stderr, "Failed to allocate output buffer\n");
        munmap(elf_map, elf_size);
        return 1;
    }
    memcpy(output_elf, elf_map, elf_size);
    embed_payload(output_elf, patched_payload, src_woody_payload_bin_len);

    Elf64_Ehdr *ehdr_dbg = (Elf64_Ehdr *)output_elf;
    Elf64_Phdr *phdr_dbg = (Elf64_Phdr *)((char *)output_elf + ehdr_dbg->e_phoff);
    for (int i = 0; i < ehdr_dbg->e_phnum; i++) {
        printf("PHDR[%d]: type=0x%x vaddr=0x%lx memsz=0x%lx filesz=0x%lx flags=0x%x\n",
            i, phdr_dbg[i].p_type, phdr_dbg[i].p_vaddr, phdr_dbg[i].p_memsz, phdr_dbg[i].p_filesz, phdr_dbg[i].p_flags);
    }

    if (write_new_elf("woody", output_elf, new_size) != 0) {
        free(output_elf);
        munmap(elf_map, elf_size);
        return 1;
    }
    free(output_elf);
    munmap(elf_map, elf_size);
    printf("Success: Encrypted ELF saved as woody\n");
    return 0;
}