#include "payload_patch.h"
#include <stdio.h>

void patch_payload(unsigned char *payload, size_t payload_size, uint64_t text_vaddr, uint64_t text_size, uint64_t orig_entry, uint64_t main_addr) {
    for (size_t i = 0; i + 8 <= payload_size; i++) {
        uint64_t *p = (uint64_t *)(payload + i);
        if (*p == 0x1111111111111111UL) {
            //printf("[patch_payload] patching TEXT_START at offset %zu\n", i);
            *p = text_vaddr;
        } else if (*p == 0x2222222222222222UL) {
            //printf("[patch_payload] patching TEXT_SIZE at offset %zu\n", i);
            *p = text_size;
        } else if (*p == 0x3333333333333333UL) {
            //printf("[patch_payload] patching ORIG_ENTRY at offset %zu\n", i);
            *p = orig_entry;
        } else if (*p == 0x4444444444444444UL) {
            //printf("[patch_payload] patching MAIN_ADDR at offset %zu\n", i);
            *p = main_addr;
        }
    }
}
