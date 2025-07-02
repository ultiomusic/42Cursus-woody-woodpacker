#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    int fd;
    off_t size;
    char *data;
    char *p, *base;

    if (argc != 2) {
        printf("usage: %s <file>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek");
        close(fd);
        exit(1);
    }
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(1);
    }

    data = malloc(size);
    if (!data) {
        perror("malloc");
        close(fd);
        exit(1);
    }

    {
        off_t total = 0;
        while (total < size) {
            ssize_t r = read(fd, data + total, size - total);
            if (r < 0) {
                perror("read");
                free(data);
                close(fd);
                exit(1);
            }
            if (r == 0) break;
            total += r;
        }
        if (total != size) {
            perror("read");
            free(data);
            close(fd);
            exit(1);
        }
    }

    close(fd);

    base = argv[1];
    for (p = argv[1]; *p; ++p) {
        if (*p == '/')
            base = p + 1;
    }

    printf("unsigned char src_");
    for (p = base; *p; ++p) {
        if (*p == '.' || *p == '-')
            printf("_");
        else
            printf("%c", *p);
    }
    printf("[] = {");

    for (off_t i = 0; i < size; ++i) {
        if (i % 12 == 0)
            printf("\n    ");
        printf("0x%02x, ", (unsigned char)data[i]);
    }

    printf("\n};\nunsigned int src_");
    for (p = base; *p; ++p) {
        if (*p == '.' || *p == '-')
            printf("_");
        else
            printf("%c", *p);
    }
    printf("_len = %lld;\n", (long long)size);

    free(data);
    return 0;
}
