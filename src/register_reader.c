#include "register_reader.h"


const struct RegisterReader zero_reader = {
    .reg = NULL,
    .read_at = zero_register_read_at,
};

const struct RegisterReader ones_reader = {
    .reg = NULL,
    .read_at = ones_register_read_at,
};

word_t zero_register_read_at(void*, size_t) {
    return 0;
}

word_t ones_register_read_at(void*, size_t) {
    return 0xff;
}

word_t normal_register_read_at(struct Register* reg, size_t index) {
    if (index >= sizeof reg->lane / sizeof *reg->lane) {
        return 0;
    }

    return reg->lane[index];
}

word_t mask_register_read_at(struct MaskRegister* reg, size_t index) {
    if (index > 8 * sizeof reg->mask / sizeof *reg->mask) {
        return 0;
    }

    size_t const byte_index = index / 8;
    size_t const bit_index = index % 8;
    bool const value = (reg->mask[byte_index] >> bit_index) & 1;
    return value ? (word_t) -1 : 0;
}

word_t register_reader_read_at(struct RegisterReader reader, size_t index) {
    return reader.read_at(reader.reg, index);
}

