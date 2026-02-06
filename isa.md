# VISA

## TODO
 - [ ] finish documentation
 - [ ] fully implement emulator
 - [ ] compiler(?)

## Info
 - 32 bit words
 - vector-first, with a max size of 256 word (32 bit lanes) for a vector register (16 * zmm!)
 - every instruction respects the mask register
 - every instruction is vectorized
 - supports both horizontal and parallel operations
 - mine :)

## Registers

### non directly accessible
 - `ip` - 32 bit. instruction pointer
 - `sp` - 32 bit. stack pointer

### non vector registers
 - `vlen` - 8 bit. is the length (lane count) of the vector registers
 - `mask` - 256 bit (2^8). mask for vector operations

### vector registers
 - `r00`
 - `r01`
 - `r02`
 - `r03`
 - `r04`
 - `r05`
 - `r06`
 - `r07`
 - `r08`
 - `r09`
 - `r10`
 - `r11`
 - `r12`
 - `r13`
 - `rf` - all zero (constant)
 - `rt` - all ones (constant)


## Instructions

### vector info registers
 - `vlenset imm8`
 - `maskset r` - `vmask[bit i] = r[byte i] != 0`

### data
 - `load  r, [r], imm2`
 - `store r, [r], imm2`
 - `copyl r, imm16`, `copyu r, imm16`

##### load/store
both load and store operations are vectorized, e.g. store: `mem[dst[i]] = src[i] if mask[i]`

the `imm2` argument specifies the amount of bytes to read/write. can be a value between 1 and 4

##### copyl / copyu
copy `imm16` to lower / upper half of every lane (is actually the same instruction, with some bit flag set).

### arithmetic
 - `add r, r, r` - dst, lhs, rhs
 - `sub r, r, r` - dst, lhs, rhs
 - `mul r, r, r` - dst, lhs, rhs
 - `div r, r, r, r` - dst, lhs, rhs, remainder

### branching
 - `jz r, imm16` - relative jmp  if entire register is 0
 - `bz r, imm16` - relative call if entire register is 0
 - `ret` - return

### bitwise
 - `and r, r, r`
 - `or  r, r, r`
 - `xor r, r, r`

### other
 - `inter imm16` - interrupt
