#pragma once

#include "register.h"
#include "instruction.h"
#include <stddef.h>


struct Cpu {
    byte_t vlen; // IMPORTANT: can't be 0, so the actual length is always vlen+1
    word_t ip;
    word_t sp;
    struct MaskRegister mask_register;
    struct Register registers[Reg_COUNT];
};


struct CpuContext {
    byte_t* memory;
    size_t memory_len;
};


void cpu_init(struct Cpu* self, word_t stack_end);

bool cpu_fetch(struct Cpu* self, struct CpuContext ctx, struct Instruction* output);

bool cpu_execute(struct Cpu* self, struct CpuContext ctx, struct Instruction inst);

bool cpu_push_word(struct Cpu* self, struct CpuContext ctx, word_t word);

bool cpu_pop_word(struct Cpu* self, struct CpuContext ctx, word_t* output);
