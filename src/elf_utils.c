#include "elf_utils.h"
#include <string.h>

void *map_and_check_elf(const char *filename, size_t *size) {
    // 1. Dosyayı aç
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error Open");
        return NULL;
    }

    // 2. ELF header'ını oku
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        fprintf(stderr, "Error: Not a valid ELF file\n");
        close(fd);
        return NULL;
    }

    // 3. ELF ve x86_64 kontrolü
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||
        ehdr.e_ident[EI_CLASS] != ELFCLASS64 ||
        ehdr.e_machine != EM_X86_64 ||
        (ehdr.e_type != ET_EXEC && ehdr.e_type != ET_DYN)) {
        fprintf(stderr, "Error: Unsupported file. x86_64 ELF binaries only\n");
        close(fd);
        return NULL;
    }

    // 4. Dosya boyutunu öğren (lseek ile)
    *size = lseek(fd, 0, SEEK_END);
    if (*size == (size_t)-1) {
        perror("lseek");
        close(fd);
        return NULL;
    }
    lseek(fd, 0, SEEK_SET);

    // 5. Memory mapping
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

