#pragma once

#include "register.h"

// exported list of instructions for easier dispatch
#define INSTRUCTIONS \
    INST(vlenset) \
    INST(maskset) \
    INST(load) \
    INST(store) \
    INST(copy) \
    INST(add) \
    INST(sub) \
    INST(mul) \
    INST(div) \
    INST(jz) \
    INST(bz) \
    INST(ret) \
    INST(and) \
    INST(or) \
    INST(xor) \
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


INSTRUCTION(vlenset, {
    // IMPORTANT: vlen is always +1, which means that a value of 0 here means 1 lane
    uint8_t value;
});

INSTRUCTION(maskset, {
    REGISTER_FIELD(src);
});

INSTRUCTION(load, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(ptr);
    uint8_t bytes_count : 2;
});

INSTRUCTION(store, {
    REGISTER_FIELD(src);
    REGISTER_FIELD(ptr);
    uint8_t _pad : 6;
    uint8_t bytes_count : 2;
});

INSTRUCTION(copy, {
    REGISTER_FIELD(dst);
    uint8_t _pad : 3;
    bool upper : 1;
    uint16_t value : 16;
});

INSTRUCTION(add, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(sub, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(mul, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(div, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    REGISTER_FIELD(remainder);
    bool horizontal : 1;
});

INSTRUCTION(jz, {
    REGISTER_FIELD(flag);
    uint16_t offset;
});

INSTRUCTION(bz, {
    REGISTER_FIELD(flag);
    uint16_t offset;
});

INSTRUCTION(ret, {
    // C doesn't allow empty structs, but `ret` should be empty.
    uint8_t _something : 1;
});

INSTRUCTION(and, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(or, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(xor, {
    REGISTER_FIELD(dst);
    REGISTER_FIELD(lhs);
    REGISTER_FIELD(rhs);
    uint8_t _pad : 4;
    bool horizontal : 1;
});

INSTRUCTION(inter, {
    uint16_t number;
});

#define INSTRUCTION_FIELD(name) struct Inst_##name name

union InstructionVariant {
    INSTRUCTION_FIELD(vlenset);
    INSTRUCTION_FIELD(maskset);

    INSTRUCTION_FIELD(load);
    INSTRUCTION_FIELD(store);
    INSTRUCTION_FIELD(copy);

    INSTRUCTION_FIELD(add);
    INSTRUCTION_FIELD(sub);
    INSTRUCTION_FIELD(mul);
    INSTRUCTION_FIELD(div);

    INSTRUCTION_FIELD(jz);
    INSTRUCTION_FIELD(bz);
    INSTRUCTION_FIELD(ret);

    INSTRUCTION_FIELD(and);
    INSTRUCTION_FIELD(or);
    INSTRUCTION_FIELD(xor);

    INSTRUCTION_FIELD(inter);
} __attribute__((packed));


struct Instruction {
    union {
        enum Opcode opcode : 4;
        union InstructionVariant variant;
    } __attribute__((packed));
} __attribute__((packed));


#undef INSTRUCTION_FIELD
#undef REGISTER_FIELD
#undef INSTRUCTION
