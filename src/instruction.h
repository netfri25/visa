#pragma once

#include "register.h"

// exported list of instructions for easier dispatch
#define INSTRUCTIONS \
    INST(vlenset) \
    INST(maskset) \
    INST(memory) \
    INST(copy) \
    INST(arith) \
    INST(bz) \
    INST(ret) \
    INST(inter)


enum Opcode {
#define INST(name) Opcode_##name,
INSTRUCTIONS
#undef INST
};


#define INSTRUCTION(name, body) \
    struct Inst_##name { \
        enum Opcode opcode : 4; \
        struct body __attribute__((packed)); \
    } __attribute__((packed))


#define REGISTER_FIELD(name) enum RegisterName name : 4
#define PAD(_bits) uint8_t : _bits


INSTRUCTION(vlenset, {
    // IMPORTANT: vlen is always +1, which means that a value of 0 here means 1 lane
    uint8_t value;
});

INSTRUCTION(maskset, {
    REGISTER_FIELD(src);
});

INSTRUCTION(memory, {
    REGISTER_FIELD(reg);
    REGISTER_FIELD(ptr);
    PAD(5);
    bool store : 1; // load/store
    uint8_t bytes_count : 2;
});

INSTRUCTION(copy, {
    REGISTER_FIELD(dst);
    PAD(3);
    bool upper : 1;
    uint16_t value : 16;
});

// TODO: do I want something similar to vpternlog?
enum Arith {
    Arith_ADD,
    Arith_SUB,
    Arith_MUL,
    Arith_DIV,
    Arith_AND,
    Arith_XOR,
};

INSTRUCTION(arith, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    REGISTER_FIELD(rem);
    bool negate_dst : 1;
    bool negate_lhs : 1;
    bool negate_rhs : 1;
    bool negate_rem : 1;
    bool horizontal : 1;
    enum Arith op : 3;
});

INSTRUCTION(bz, {
    REGISTER_FIELD(flag);
    PAD(3);
    bool save_ip : 1;
    uint16_t offset;
});

INSTRUCTION(ret, {
    // C doesn't allow empty structs, but `ret` should be empty.
    uint8_t _ : 1;
});

INSTRUCTION(inter, {
    uint16_t number;
});

#define INSTRUCTION_FIELD(name) struct Inst_##name name

union InstructionVariant {
    INSTRUCTION_FIELD(vlenset);
    INSTRUCTION_FIELD(maskset);

    INSTRUCTION_FIELD(memory);
    INSTRUCTION_FIELD(copy);

    INSTRUCTION_FIELD(arith);

    INSTRUCTION_FIELD(bz);
    INSTRUCTION_FIELD(ret);

    INSTRUCTION_FIELD(inter);
} __attribute__((packed));


struct Instruction {
    union {
        enum Opcode opcode : 4;
        union InstructionVariant variant;
    } __attribute__((packed));
} __attribute__((packed));


#undef INSTRUCTION_FIELD
#undef PAD
#undef REGISTER_FIELD
#undef INSTRUCTION
