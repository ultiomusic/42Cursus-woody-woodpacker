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
