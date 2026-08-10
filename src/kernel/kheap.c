#include "kheap.h"

extern uint8_t _end;

#define HEAP_SIZE (4 * 1024 * 1024)

static uintptr_t heap_start = 0;
static uintptr_t heap_current = 0;
static uintptr_t heap_end = 0;

void kheap_init(void) {
    heap_start = (uintptr_t)&_end;
    heap_start = (heap_start + 4095) & ~((uintptr_t)4095);
    heap_current = heap_start;
    heap_end = heap_start + HEAP_SIZE;
}

void *kmalloc_aligned(size_t size, size_t alignment) {
    if (heap_start == 0) {
        kheap_init();
    }

    if (alignment < 16) alignment = 16;

    uintptr_t aligned_addr = (heap_current + (alignment - 1)) & ~(alignment - 1);

    if (aligned_addr + size > heap_end) {
        return NULL;
    }

    heap_current = aligned_addr + size;
    return (void *)aligned_addr;
}

void *kmalloc(size_t size) {
    return kmalloc_aligned(size, 16);
}