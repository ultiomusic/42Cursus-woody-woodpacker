#include "writer.h"

void write_new_elf(const char *output_file, void *elf_map, size_t size) {
    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        perror("open");
        return;
    }

    ssize_t written = write(fd, elf_map, size);
    if (written != (ssize_t)size) {
        if (written < 0) {
            perror("write");
        } else {
            printf("Warning: wrote only %zd bytes of %zu\n", written, size);
        }
    }

    close(fd);
}
