#include "payload_patch.h"
#include <stdio.h>

void patch_payload(unsigned char *payload, size_t payload_size, uint64_t text_vaddr, uint64_t text_size, uint64_t orig_entry, uint64_t main_addr) {
    printf("[patch_payload] text_vaddr=0x%lx text_size=0x%lx orig_entry=0x%lx main_addr=0x%lx\n", text_vaddr, text_size, orig_entry, main_addr);
    uint64_t *p = (uint64_t *)payload;
    size_t n = payload_size / 8;
    for (size_t i = 0; i < n; i++) {
        if (p[i] == 0x1111111111111111UL) {
            printf("[patch_payload] patching TEXT_START at offset %zu\n", i * 8);
            p[i] = text_vaddr;
        } else if (p[i] == 0x2222222222222222UL) {
            printf("[patch_payload] patching TEXT_SIZE at offset %zu\n", i * 8);
            p[i] = text_size;
        } else if (p[i] == 0x3333333333333333UL) {
            printf("[patch_payload] patching ORIG_ENTRY at offset %zu\n", i * 8);
            p[i] = orig_entry;
        } else if (p[i] == 0x4444444444444444UL) {
            printf("[patch_payload] patching MAIN_ADDR at offset %zu\n", i * 8);
            p[i] = main_addr;
        }
    }
}
