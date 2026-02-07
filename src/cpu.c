#include "cpu.h"

#include "register_reader.h"
#include "register_writer.h"

#include <string.h>
#include <assert.h>

static struct RegisterReader cpu_get_mask_register_reader(struct Cpu* self) {
    return (struct RegisterReader) {
        .reg = &self->mask_register,
        .read_at = (RegisterReader_read_at_t) mask_register_read_at,
    };
}

static struct RegisterWriter cpu_get_mask_register_writer(struct Cpu* self) {
    return (struct RegisterWriter) {
        .reg = &self->mask_register,
        .write_at = (RegisterWriter_write_at_t) mask_register_write_at,
    };
}


static struct RegisterReader cpu_get_register_reader(struct Cpu* self, enum RegisterName reg_name) {
    if (reg_name == Reg_RF) {
        return zero_reader;
    }

    if (reg_name == Reg_RT) {
        return ones_reader;
    }

    return (struct RegisterReader) {
        .reg = &self->registers[reg_name],
        .read_at = (RegisterReader_read_at_t) normal_register_read_at,
    };
}

static struct RegisterWriter cpu_get_register_writer(struct Cpu* self, enum RegisterName reg_name) {
    if (reg_name == Reg_RF || reg_name == Reg_RT) {
        return no_register_writer;
    }

    return (struct RegisterWriter) {
        .reg = &self->registers[reg_name],
        .write_at = (RegisterWriter_write_at_t) normal_register_write_at,
    };
}


#define VECTORIZE(_cpu, _index, _body) \
    struct RegisterReader mask_register_reader = cpu_get_mask_register_reader(_cpu); \
    for (size_t _index = 0; _index < (size_t) (_cpu)->vlen + 1; _index++) { \
        if (register_reader_read_at(mask_register_reader, _index)) _body \
    }


static inline bool cpu_execute_vlenset(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_vlenset inst
) {
    (void)! ctx;
    self->vlen = inst.value;
    return true;
}

static inline bool cpu_execute_maskset(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_maskset inst
) {
    (void)! ctx;

    struct RegisterReader reader = cpu_get_register_reader(self, inst.src);
    struct RegisterWriter writer = cpu_get_mask_register_writer(self);

    for (size_t i = 0; i < (size_t) self->vlen + 1; i++) {
        word_t const value = register_reader_read_at(reader, i);
        register_writer_write_at(writer, i, value);
    }

    return true;
}

static inline bool cpu_execute_load(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_load inst
) {
    struct RegisterReader ptr_reader = cpu_get_register_reader(self, inst.ptr);
    struct RegisterWriter dst_writer = cpu_get_register_writer(self, inst.dst);

    VECTORIZE(self, i, {
        word_t value = 0;
        byte_t* const ptr = ctx->memory + register_reader_read_at(ptr_reader, i);
        memcpy(&value, ptr, inst.bytes_count);
        register_writer_write_at(dst_writer, i, value);
    });

    return true;
}

static inline bool cpu_execute_store(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_store inst
) {
    struct RegisterReader ptr_reader = cpu_get_register_reader(self, inst.ptr);
    struct RegisterReader src_reader = cpu_get_register_reader(self, inst.src);

    VECTORIZE(self, i, {
        word_t const value = register_reader_read_at(src_reader, i);
        byte_t* const ptr = ctx->memory + register_reader_read_at(ptr_reader, i);
        memcpy(ptr, &value, inst.bytes_count);
    });

    return true;
}

static inline bool cpu_execute_copy(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_copy inst
) {
    struct RegisterWriter dst_writer = cpu_get_register_writer(self, inst.dst);

    VECTORIZE(self, i, {
        register_writer_write_at(dst_writer, i, (word_t)(inst.value) << (16 * inst.upper));
    });

    return true;
}

static inline bool cpu_execute_add(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_add inst
) {
    assert(!inst.horizontal && "TODO: horizontal operations not implemented");
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_sub(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_sub inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_mul(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_mul inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_div(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_div inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_jz(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_jz inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_bz(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_bz inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_ret(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_ret inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_and(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_and inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_or(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_or inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_xor(
    struct Cpu* self,
    const struct CpuContext* ctx,
    struct Inst_xor inst
) {
    assert(false && "TODO: not implemented");
}

static inline bool cpu_execute_inter(
    struct Cpu* self,
    const struct CpuContext* ctx,
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

bool cpu_fetch(struct Cpu* self, const struct CpuContext* ctx, struct Instruction* output) {
    if (self->ip >= ctx->memory_len) {
        assert(false && "TODO: not implemented");
    }

    void* ptr = ctx->memory + self->ip;
    memcpy(output, ptr, sizeof *output);

    self->ip += sizeof *output;

    return true;
}

bool cpu_execute(struct Cpu* self, const struct CpuContext* ctx, struct Instruction inst) {
    switch (inst.opcode) {
#define INST(name) case Opcode_##name: return cpu_execute_##name(self, ctx, inst.variant.name);
        INSTRUCTIONS
#undef INST
    default: assert(false && "TODO: not implemented");
    }
}

bool cpu_push_word(struct Cpu* self, struct CpuContext* ctx, word_t word) {
    if (self->sp < sizeof word) {
        assert(false && "TODO: not implemented");
    }

    self->sp -= sizeof word;
    *(word_t*)(ctx->memory + self->sp) = word;
    return true;
}

bool cpu_pop_word(struct Cpu* self, struct CpuContext* ctx, word_t* output) {
    // FIXME: I'm not really sure how to check for the stack pointer, so for now it'll be left unchecked.
    *output = *(word_t*)(ctx->memory + self->sp);
    self->sp += sizeof *output;
    return true;
}
