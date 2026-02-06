#pragma once

#include "types.h"

enum RegisterName : byte_t {
    Reg_R00 = 0x0,
    Reg_R01 = 0x1,
    Reg_R02 = 0x2,
    Reg_R03 = 0x3,
    Reg_R04 = 0x4,
    Reg_R05 = 0x5,
    Reg_R06 = 0x6,
    Reg_R07 = 0x7,
    Reg_R08 = 0x8,
    Reg_R09 = 0x9,
    Reg_R10 = 0xa,
    Reg_R11 = 0xb,
    Reg_R12 = 0xc,
    Reg_R13 = 0xd,
    Reg_RF  = 0xe, // constant 0
    Reg_RT  = 0xf, // constant 1
};

#define Reg_COUNT 0xe
#define Reg_IS_CONST(reg) ((reg) & 0b111)


#define VECTOR_MAX_LANES_BITS 8
#define VECTOR_MAX_LANES (1 << VECTOR_MAX_LANES_BITS)

struct Register {
    word_t lane[VECTOR_MAX_LANES];
};

struct MaskRegister {
    uint8_t mask[VECTOR_MAX_LANES * sizeof(word_t) / 8];
};

