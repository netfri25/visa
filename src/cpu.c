#include "cpu.h"

#include <string.h>
#include <assert.h>

#define VECTORIZE(_cpu, _index, _body) \
    do { \
        for (size_t _index = 0; _index < (size_t)((_cpu)->vlen) + 1; _index++) { \
            if (cpu_mask_register_read(_cpu, _index)) _body \
        } \
    } while (0)


static inline bool cpu_mask_register_read(struct Cpu* self, size_t index) {
    if (index > 8 * sizeof self->mask_register.mask / sizeof *self->mask_register.mask) {
        return 0;
    }

    size_t const byte_index = index / 8;
    size_t const bit_index = index % 8;
    bool const value = (self->mask_register.mask[byte_index] >> bit_index) & 1;
    return value;
}

static inline void cpu_mask_register_write(struct Cpu* self, size_t index, bool value) {
    if (index > 8 * sizeof self->mask_register.mask / sizeof *self->mask_register.mask) {
        return;
    }

    size_t const byte_index = index / 8;
    size_t const bit_index = index % 8;
    self->mask_register.mask[byte_index] |= (value != 0) << bit_index;
}

static inline word_t cpu_register_read(struct Cpu* self, enum RegisterName reg_name, size_t lane) {
    if (reg_name == Reg_RF) return 0;
    if (reg_name == Reg_RT) return (word_t) -1;

    struct Register* const reg = &self->registers[reg_name];
    if (lane >= sizeof reg->lane / sizeof *reg->lane) {
        return 0;
    }

    return reg->lane[lane];
}

static inline void cpu_register_write(struct Cpu* self, enum RegisterName reg_name, size_t lane, word_t value) {
    if (reg_name == Reg_RF || reg_name == Reg_RT) return;

    struct Register* const reg = &self->registers[reg_name];
    if (lane >= sizeof reg->lane / sizeof *reg->lane) {
        return;
    }

    reg->lane[lane] = value;
}

static inline bool cpu_is_register_zero(struct Cpu* self, enum RegisterName reg_name) {
    VECTORIZE(self, i, {
        if (cpu_register_read(self, reg_name, i) != 0) {
            return false;
        }
    });

    return true;
}

static inline bool cpu_execute_vlenset(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_vlenset inst
) {
    (void) ctx;
    self->vlen = inst.value;
    return true;
}

static inline bool cpu_execute_maskset(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_maskset inst
) {
    (void) ctx;

    for (size_t i = 0; i < (size_t) self->vlen + 1; i++) {
        word_t const value = cpu_register_read(self, inst.src, i);
        cpu_mask_register_write(self, i, value != 0);
    }

    return true;
}

static inline bool cpu_execute_memory(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_memory inst
) {
    if (inst.store) {
        VECTORIZE(self, i, {
            word_t const value = cpu_register_read(self, inst.reg, i);
            byte_t* const ptr = ctx.memory + cpu_register_read(self, inst.ptr, i);
            memcpy(ptr, &value, inst.bytes_count);
        });
    } else {
        VECTORIZE(self, i, {
            word_t value = 0;
            byte_t* const ptr = ctx.memory + cpu_register_read(self, inst.ptr, i);
            memcpy(&value, ptr, inst.bytes_count);
            cpu_register_write(self, inst.reg, i, value);
        });
    }

    return true;
}

static inline bool cpu_execute_copy(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_copy inst
) {
    (void) ctx;

    VECTORIZE(self, i, {
        word_t const value = (word_t)(inst.value) << (16 * inst.upper);
        cpu_register_write(self, inst.dst, i, value);
    });

    return true;
}

typedef word_t (*arith_op_t)(word_t lhs, word_t rhs, word_t* rem);

static word_t arith_op_add(word_t lhs, word_t rhs, word_t* rem) {
    (void)! rem;
    return lhs + rhs;
}

static word_t arith_op_sub(word_t lhs, word_t rhs, word_t* rem) {
    (void)! rem;
    return lhs - rhs;
}

static word_t arith_op_mul(word_t lhs, word_t rhs, word_t* rem) {
    // FIXME: it's popular to allow 2*word multiplication for the lhs. should this support it?
    (void)! rem;
    return lhs * rhs;
}

static word_t arith_op_div(word_t lhs, word_t rhs, word_t* rem) {
    (void)! rem;

    if (rhs == 0) {
        *rem = 0;
        return 0;
    }

    *rem = lhs % rhs;
    return lhs / rhs;
}

static word_t arith_op_and(word_t lhs, word_t rhs, word_t* rem) {
    (void)! rem;
    return lhs & rhs;
}

static word_t arith_op_xor(word_t lhs, word_t rhs, word_t* rem) {
    (void)! rem;
    return lhs ^ rhs;
}

static inline arith_op_t get_arith_op(enum Arith op) {
    switch (op) {
    case Arith_ADD: return arith_op_add;
    case Arith_SUB: return arith_op_sub;
    case Arith_MUL: return arith_op_mul;
    case Arith_DIV: return arith_op_div;
    case Arith_AND: return arith_op_and;
    case Arith_XOR: return arith_op_xor;
    }
}

static inline bool cpu_execute_arith(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_arith inst
) {
    (void) ctx;

    assert(!inst.horizontal && "TODO: horizontal operations not implemented");

    arith_op_t const op = get_arith_op(inst.op);

    VECTORIZE(self, i, {
        word_t lhs = cpu_register_read(self, inst.lhs, i);
        if (inst.negate_lhs) lhs = ~lhs;

        word_t rhs = cpu_register_read(self, inst.rhs, i);
        if (inst.negate_rhs) rhs = ~rhs;

        word_t rem = 0;
        word_t dst = op(lhs, rhs, &rem);
        if (inst.negate_rem) rem = ~rem;
        if (inst.negate_dst) dst = ~dst;

        cpu_register_write(self, inst.dst, i, dst);
        cpu_register_write(self, inst.rem, i, rem);
    });

    return true;
}

static inline bool cpu_execute_jz(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_jz inst
) {
    (void) ctx;

    if (cpu_is_register_zero(self, inst.flag)) {
        self->ip += inst.offset;
    }

    return true;
}

static inline bool cpu_execute_bz(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_bz inst
) {
    if (cpu_is_register_zero(self, inst.flag)) {
        if (!cpu_push_word(self, ctx, self->ip)) {
            return false;
        }

        self->ip += inst.offset;
    }

    return true;
}

static inline bool cpu_execute_ret(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_ret inst
) {
    (void) inst;

    word_t value;
    if (!cpu_pop_word(self, ctx, &value)) {
        return false;
    }

    self->ip = value;
    return true;
}

static inline bool cpu_execute_inter(
    struct Cpu* self,
    struct CpuContext ctx,
    struct Inst_inter inst
) {
    assert(false && "TODO: not implemented");
}


void cpu_init(struct Cpu* self, word_t stack_end) {
    self->vlen = 0xff; // max value
    self->ip = 0x0000'0000;
    self->sp = stack_end;
    memset(&self->mask_register, 0xff, sizeof self->mask_register);
    memset(&self->registers, 0x00, sizeof self->registers);
}

bool cpu_fetch(struct Cpu* self, struct CpuContext ctx, struct Instruction* output) {
    if (self->ip >= ctx.memory_len) {
        assert(false && "TODO: not implemented");
    }

    void* ptr = ctx.memory + self->ip;
    memcpy(output, ptr, sizeof *output);

    self->ip += sizeof *output;

    return true;
}

bool cpu_execute(struct Cpu* self, struct CpuContext ctx, struct Instruction inst) {
    switch (inst.opcode) {
#define INST(name) case Opcode_##name: return cpu_execute_##name(self, ctx, inst.variant.name);
        INSTRUCTIONS
#undef INST
    default: assert(false && "TODO: not implemented");
    }
}

bool cpu_push_word(struct Cpu* self, struct CpuContext ctx, word_t word) {
    if (self->sp < sizeof word) {
        assert(false && "TODO: not implemented");
    }

    self->sp -= sizeof word;
    *(word_t*)(ctx.memory + self->sp) = word;
    return true;
}

bool cpu_pop_word(struct Cpu* self, struct CpuContext ctx, word_t* output) {
    // FIXME: I'm not really sure how to check for the stack pointer, so for now it'll be left unchecked.
    *output = *(word_t*)(ctx.memory + self->sp);
    self->sp += sizeof *output;
    return true;
}
