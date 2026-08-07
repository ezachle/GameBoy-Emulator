#pragma once
#include "common.hpp"

typedef struct registers_t {
    union {
        uint16_t af = 0;
        struct {
            uint8_t f;
            uint8_t a;
        };
    };

    union {
        uint16_t bc = 0;
        struct {
            uint8_t c;
            uint8_t b;
        };
    };

    union {
        uint16_t de = 0;
        struct {
            uint8_t e;
            uint8_t d;
        };
    };

    union {
        uint16_t hl = 0;
        struct {
            uint8_t l;
            uint8_t h;
        };
    };

    uint16_t sp = 0;
    uint16_t pc = 0x0100;
} Registers_t;

constexpr uint8_t ZERO_FLAG       = (1 << 7);
constexpr uint8_t SUB_FLAG        = (1 << 6); // used for BCD
constexpr uint8_t HALF_CARRY_FLAG = (1 << 5); // used for BCD
constexpr uint8_t CARRY_FLAG      = (1 << 4);

typedef enum { REG_B = 0, REG_C, REG_D, REG_E, REG_H, REG_L, REG_HL, REG_A } regs_t;

