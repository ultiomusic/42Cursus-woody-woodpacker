#include "elf_utils.h"
#include <string.h>

void *map_and_check_elf(const char *filename, size_t *size) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return NULL;
    }

    // 1. Tüm ELF header'ı tek seferde oku
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
        perror("read");
        close(fd);
        return NULL;
    }

    // 2. Tek seferde tüm kontroller:
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0 ||  // ELF mi?
       ehdr.e_ident[EI_CLASS] != ELFCLASS64 ||         // 64-bit mi?
       ehdr.e_machine != EM_X86_64 ||                 // x86_64 mü?
       (ehdr.e_type != ET_EXEC && ehdr.e_type != ET_DYN)) { // Çalıştırılabilir/shared mi?
        
        fprintf(stderr, "File architecture not suported. x86_64 ELF only.\n");
        close(fd);
        return NULL;
    }

    // 3. Dosya boyutunu al ve memory-map'le
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return NULL;
    }

    *size = st.st_size;
    void *map = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    return (map == MAP_FAILED) ? NULL : map;
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
