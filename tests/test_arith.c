#include "test.h"

struct Cpu* init(void) {
    struct Cpu* cpu = malloc(sizeof *cpu);
    cpu_init(cpu, 0);

    random_init_buffer(cpu->registers[Reg_R01].lane, VECTOR_MAX_LANES);
    random_init_buffer(cpu->registers[Reg_R02].lane, VECTOR_MAX_LANES);

    cpu->vlen = VECTOR_MAX_LANES - 1;

    return cpu;
}

void test_add(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .inverse = false,
        .product = false,
    };

    cpu_execute_arith(cpu, NULL, inst);

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert(lhs + rhs == res);
    }
}

void test_sub(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .inverse = true,
        .product = false,
    };

    cpu_execute_arith(cpu, NULL, inst);

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert(lhs - rhs == res);
    }
}

void test_mul(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .inverse = false,
        .product = true,
    };

    cpu_execute_arith(cpu, NULL, inst);

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert(lhs * rhs == res);
    }
}

void test_div(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_R03,
        .inverse = true,
        .product = true,
    };

    cpu_execute_arith(cpu, NULL, inst);

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        word_t const rem = cpu->registers[Reg_R03].lane[i];
        if (rhs == 0) {
            assert(res == 0);
            assert(rem == 0);
        } else {
            assert(lhs / rhs == res);
            assert(lhs % rhs == rem);
        }
    }
}

int main(void) {
    srand(time(NULL));

    struct Cpu* cpu = init();
    test_add(cpu);
    test_sub(cpu);
    test_mul(cpu);
    test_div(cpu);

    return 0;
}
