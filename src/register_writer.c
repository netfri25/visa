#include "register_writer.h"


const struct RegisterWriter no_register_writer = {
    .reg = NULL,
    .write_at = (RegisterWriter_write_at_t) no_register_write_at,
};


void no_register_write_at(void*, size_t, word_t) {
    return;
}

void mask_register_write_at(struct MaskRegister* reg, size_t index, word_t value) {
    if (index > 8 * sizeof reg->mask / sizeof *reg->mask) {
        return;
    }

    size_t const byte_index = index / 8;
    size_t const bit_index = index % 8;
    reg->mask[byte_index] |= (value != 0) << bit_index;
}

void normal_register_write_at(struct Register* reg, size_t index, word_t value) {
    if (index >= sizeof reg->lane / sizeof *reg->lane) {
        return;
    }

    reg->lane[index] = value;
}

void register_writer_write_at(struct RegisterWriter writer, size_t index, word_t value) {
    writer.write_at(writer.reg, index, value);
}
