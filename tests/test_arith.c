#include "test.h"

struct Cpu* init(void) {
    struct Cpu* cpu = xmalloc(sizeof *cpu);
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
        .op = Arith_ADD,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

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
        .op = Arith_SUB,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

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
        .op = Arith_MUL,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

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
        .op = Arith_DIV,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

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

void test_nand(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .op = Arith_AND,
        .negate_dst = true,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert(~(lhs & rhs) == res);
    }
}

void test_or(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .op = Arith_AND,
        .negate_dst = true,
        .negate_lhs = true,
        .negate_rhs = true,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert((lhs | rhs) == res);
    }
}

void test_xor(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_R02,
        .rem = Reg_RF,
        .op = Arith_XOR,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const rhs = cpu->registers[Reg_R02].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert((lhs ^ rhs) == res);
    }
}

void test_not(struct Cpu* cpu) {
    struct Inst_arith const inst = {
        .dst = Reg_R00,
        .lhs = Reg_R01,
        .rhs = Reg_RT,
        .rem = Reg_RF,
        .op = Arith_XOR,
        .negate_dst = false,
        .negate_lhs = false,
        .negate_rhs = false,
        .negate_rem = false,
    };

    assert(cpu_execute_arith(cpu, (struct CpuContext){}, inst));

    for (size_t i = 0; i <= cpu->vlen; i++) {
        word_t const lhs = cpu->registers[Reg_R01].lane[i];
        word_t const res = cpu->registers[Reg_R00].lane[i];
        assert(~lhs == res);
    }
}

int main(void) {
    srand(time(NULL));

    struct Cpu* cpu = init();
    test_add(cpu);
    test_sub(cpu);
    test_mul(cpu);
    test_div(cpu);
    test_nand(cpu);
    test_or(cpu);
    test_xor(cpu);
    test_not(cpu);

    return 0;
}
