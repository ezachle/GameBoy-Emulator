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