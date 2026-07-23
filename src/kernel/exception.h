#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>

// Matches the 256-byte register frame pushed on the stack by save_registers
typedef struct {
    uint64_t x[31];
} register_frame_t;

void handle_sync_exception(register_frame_t *frame);
void handle_invalid_exception(register_frame_t *frame);

#endif