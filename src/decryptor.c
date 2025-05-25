#include <stdlib.h>

void decrypt(unsigned char *text, size_t text_size, unsigned char *key, size_t key_size) {
    size_t key_offset;
    size_t key_index;
    size_t i;

    key_index = 0;
    key_offset = 0;
    i = 0;
    while (i < text_size)
    {
        text[i] ^= key[key_index] + key_offset;
        key_index++;
        if (key_index >= key_size) 
        {
            key_index = 0;
            key_offset += 42;
        }
        i++;
    }
}