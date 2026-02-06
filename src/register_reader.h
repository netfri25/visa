#pragma once

#include <stddef.h>

#include "register.h"

typedef word_t (*RegisterReader_read_at_t)(void* reg, size_t index);

struct RegisterReader {
    void* reg;
    RegisterReader_read_at_t read_at;
};

word_t zero_register_read_at(void*, size_t);
word_t ones_register_read_at(void*, size_t);
word_t normal_register_read_at(struct Register* reg, size_t index);
word_t mask_register_read_at(struct MaskRegister* reg, size_t index);

word_t register_reader_read_at(struct RegisterReader reader, size_t index);


extern const struct RegisterReader zero_reader;
extern const struct RegisterReader ones_reader;
