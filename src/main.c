#define _GNU_SOURCE
#include "encryption.h"
#include "writer.h"
#include "embed.h"
#include "payload_patch.h"
#include "woody_payload.h"
#include "woody_payload.inc"
#include <sys/mman.h>
#include <stdlib.h>

unsigned char g_key_value;
unsigned char g_key[KEY_SIZE];

void *map_and_check_elf(const char *filename, size_t *size) {

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error Open");
        return NULL;
    }

    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        fprintf(stderr, "Error: Not a valid ELF file\n");
        close(fd);
        return NULL;
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||
        ehdr.e_ident[EI_CLASS] != ELFCLASS64 ||
        ehdr.e_machine != EM_X86_64 ||
        (ehdr.e_type != ET_EXEC && ehdr.e_type != ET_DYN)) {
        fprintf(stderr, "Error: Unsupported file. x86_64 ELF binaries only\n");
        close(fd);
        return NULL;
    }

    *size = lseek(fd, 0, SEEK_END);
    if (*size == (size_t)-1) {
        perror("lseek");
        close(fd);
        return NULL;
    }
    lseek(fd, 0, SEEK_SET);

    void *map = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    if (map == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return map;
}

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

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_map;
    Elf64_Addr orig_entry = ehdr->e_entry;
    unsigned char patched_payload[src_woody_payload_bin_len];
    memcpy(patched_payload, src_woody_payload_bin, src_woody_payload_bin_len);

    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)elf_map + ehdr->e_shoff);
    const char *shstrtab = (char *)elf_map + shdr[ehdr->e_shstrndx].sh_offset;
    Elf64_Addr text_addr = 0;
    Elf64_Xword text_size = 0;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(shstrtab + shdr[i].sh_name, ".text") == 0) {
            text_addr = shdr[i].sh_addr;
            text_size = shdr[i].sh_size;
            break;
        }
    }
    if (!text_addr || !text_size) {
        fprintf(stderr, "Error: .text section not found\n");
        munmap(elf_map, elf_size);
        return 1;
    }
    if (encrypt_text_section(elf_map) != 0) {
        fprintf(stderr, "Failed to encrypt .text section\n");
        munmap(elf_map, elf_size);
        return 1;
    }
    Elf64_Addr main_addr = 0;
    Elf64_Shdr *symtab = NULL, *strtab = NULL;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB)
            symtab = &shdr[i];
        if (shdr[i].sh_type == SHT_STRTAB && i != ehdr->e_shstrndx)
            strtab = &shdr[i];
    }
    if (symtab && strtab) {
        const char *strs = (char *)elf_map + strtab->sh_offset;
        Elf64_Sym *syms = (Elf64_Sym *)((char *)elf_map + symtab->sh_offset);
        int nsyms = symtab->sh_size / sizeof(Elf64_Sym);
        for (int i = 0; i < nsyms; i++) {
            if (strcmp(strs + syms[i].st_name, "main") == 0) {
                main_addr = syms[i].st_value;
                break;
            }
        }
    }
    if (!main_addr) {
        fprintf(stderr, "Error: main symbol not found\n");
        munmap(elf_map, elf_size);
        return 1;
    }
    patch_payload(patched_payload, src_woody_payload_bin_len, text_addr, text_size, orig_entry, main_addr, g_key_value);

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
        if (phdr_dbg[i].p_type == PT_LOAD &&
            text_addr >= phdr_dbg[i].p_vaddr &&
            text_addr < phdr_dbg[i].p_vaddr + phdr_dbg[i].p_memsz)
        {
            phdr_dbg[i].p_flags |= PF_W;
        }
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