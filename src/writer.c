#include "writer.h"

int write_new_elf(const char *output_file, void *elf_map, size_t size) {
    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    ssize_t written = write(fd, elf_map, size);
    close(fd);

    if (written != (ssize_t)size) {
        if (written < 0) {
            perror("write");
        } else {
            fprintf(stderr, "Partial write: %zd/%zu bytes\n", written, size);
        }
        return -1;
    }

    return 0; // Başarılı
}
