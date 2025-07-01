#ifndef PAYLOAD_PATCH_H
#define PAYLOAD_PATCH_H

#include <stddef.h>
#include <stdint.h>

void patch_payload(unsigned char *payload, size_t payload_size, uint64_t text_vaddr, uint64_t text_size, uint64_t orig_entry, uint64_t main_addr, unsigned char key);

#endif
