#pragma once

#include <string>
#include <array>
#include <cstdint>

typedef struct {
    std::string instr_name;
    uint8_t length;
    uint8_t cycles;
} InstrInfo_t;

extern std::array<InstrInfo_t, 0x100> instr_lut;

typedef enum { REG_B = 0, REG_C, REG_D, REG_E, REG_H, REG_L, REG_X, REG_A } regs_t;

#define GET_SRC(opcode) (opcode & 0x7)
#define GET_DST(opcode) ((opcode & 0x38) >> 3)

#define GET_BYTE(buffer, pc) (buffer[pc+1])
#define GET_WORD(buffer, pc) ((buffer[pc+2] << 8) | (buffer[pc+1]))
