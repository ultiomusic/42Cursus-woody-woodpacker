#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(f);
        return 1;
    }
    long size = ftell(f);
    if (size < 0) {
        perror("ftell");
        fclose(f);
        return 1;
    }
    rewind(f);
    unsigned char *data = malloc(size);
    if (!data) {
        perror("malloc");
        fclose(f);
        return 1;
    }
    if (fread(data, 1, size, f) != (size_t)size) {
        perror("fread");
        fclose(f);
        free(data);
        return 1;
    }
    fclose(f);
    const char *base = strrchr(argv[1], '/');
    base = base ? base + 1 : argv[1];
    char var[256];
    snprintf(var, sizeof(var), "src_%s", base);
    for (char *p = var; *p; ++p) {
        if (*p == '.' || *p == '-')
            *p = '_';
    }
    printf("unsigned char %s[] = {", var);
    for (long i = 0; i < size; ++i) {
        if (i % 12 == 0)
            printf("\n    ");
        printf("0x%02x, ", data[i] & 0xff);
    }
    printf("\n};\nunsigned int %s_len = %ld;\n", var, size);
    free(data);
    return 0;
}