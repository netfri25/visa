#include "test.h"

word_t indices[] = { 0, 129, 12, 36, 42, 57, 68, 99, 103, 152 };
word_t values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

struct Cpu* init(void) {
    struct Cpu* cpu = xmalloc(sizeof *cpu);
    cpu_init(cpu, 0);

    srand(time(NULL));

    cpu->vlen = 1 + sizeof indices / sizeof *indices;
    memcpy(cpu->registers[Reg_R00].lane, indices, sizeof indices);
    memcpy(cpu->registers[Reg_R01].lane, values, sizeof values);

    return cpu;
}

void test_write(struct Cpu* cpu, struct CpuContext ctx) {
    struct Inst_memory const inst = {
        .ptr = Reg_R00,
        .reg = Reg_R01,
        .bytes_count = 3, // actually 4, not 3
        .store = true,
    };

    assert(cpu_execute_memory(cpu, ctx, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        void* const ptr = ctx.memory + indices[i];
        word_t const actual = *(word_t*) ptr;
        word_t const expected = cpu_register_read(cpu, Reg_R01, i);
        assert(actual == expected);
    }
}

void test_read(struct Cpu* cpu, struct CpuContext ctx) {
    struct Inst_memory const inst = {
        .ptr = Reg_R00,
        .reg = Reg_R01,
        .bytes_count = 3, // actually 4, not 3
        .store = false,
    };

    assert(cpu_execute_memory(cpu, ctx, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const index = cpu_register_read(cpu, Reg_R00, i);
        word_t const expected = *(word_t*)(ctx.memory + index);
        word_t const actual = cpu_register_read(cpu, Reg_R01, i);
        assert(actual == expected);
    }
}

int main(void) {
    struct Cpu* cpu = init();
    struct CpuContext ctx = create_ctx();

    test_write(cpu, ctx);

    shuffle_buffer(cpu->registers[Reg_R00].lane, cpu->vlen + 1);
    test_read(cpu, ctx);

    return 0;
}
