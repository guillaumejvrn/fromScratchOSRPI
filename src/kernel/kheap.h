#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

// Initialize the heap just after the end of kernel code (_end)
void kheap_init(void);

// Basic allocator (default alignment is 8 bytes)
void *kmalloc(size_t size);

// Aligned allocator: returns an address aligned to 'alignment'
void *kmalloc_aligned(size_t size, size_t alignment);

#endif