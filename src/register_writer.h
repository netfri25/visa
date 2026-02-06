#pragma once

#include <stddef.h>

#include "register.h"


typedef void (*RegisterWriter_write_at_t)(void* reg, size_t index, word_t value);

struct RegisterWriter {
    void* reg;
    RegisterWriter_write_at_t write_at;
};

void no_register_write_at(void*, size_t, word_t);
void mask_register_write_at(struct MaskRegister* reg, size_t index, word_t value);
void normal_register_write_at(struct Register* reg, size_t index, word_t value);


void register_writer_write_at(struct RegisterWriter writer, size_t index, word_t value);

extern const struct RegisterWriter no_register_writer;
