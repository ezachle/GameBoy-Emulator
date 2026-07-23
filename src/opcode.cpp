#include "opcode.hpp"

static std::array<InstrInfo_t, 0x100> init_lut() {
    std::array<InstrInfo_t, 0x100> lut {};

    // Block 0
    /*
    ld, inc, dec, add,
    rlca, rrca, rla, rra, daa, cpl, scf, ccf,
    jr
    stop
    */
    lut[0x00] = {"NOP", 1, 4};

    lut[0x01] = {"LD BC, n16", 3, 12};
    lut[0x11] = {"LD DE, n16", 3, 12};
    lut[0x21] = {"LD HL, n16", 3, 12};
    lut[0x31] = {"LD SP, n16", 3, 12};

    lut[0x02] = {"LD [BC], A", 1, 8};
    lut[0x12] = {"LD [DE], A", 1, 8};
    lut[0x22] = {"LD [HL+], A", 1, 8};
    lut[0x32] = {"LD [HL-], A", 1, 8};

    lut[0x06] = {"LD B, n8", 2, 8};
    lut[0x16] = {"LD D, n8", 2, 8};
    lut[0x26] = {"LD H, n8", 2, 8};
    lut[0x36] = {"LD [HL], n8", 2, 12};

    lut[0x08] = {"LD [a16], SP", 3, 20};

    lut[0x0A] = {"LD A, [BC]", 1, 8};
    lut[0x1A] = {"LD A, [DE]", 1, 8};
    lut[0x2A] = {"LD A, [HL+]", 1, 8};
    lut[0x3A] = {"LD A, [HL-]", 1, 8};

    lut[0x0E] = {"LD C, n8", 2, 8};
    lut[0x1E] = {"LD E, n8", 2, 8};
    lut[0x2E] = {"LD L, n8", 2, 8};
    lut[0x3E] = {"LD A, n8", 2, 8};

    lut[0x03] = {"INC BC", 1, 8};
    lut[0x13] = {"INC DE", 1, 8};
    lut[0x23] = {"INC HL", 1, 8};
    lut[0x33] = {"INC SP", 1, 8};

    lut[0x04] = {"INC B", 1, 4};
    lut[0x14] = {"INC D", 1, 4};
    lut[0x24] = {"INC H", 1, 4};
    lut[0x34] = {"INC [HL]", 1, 12};

    lut[0x0C] = {"INC C", 1, 4};
    lut[0x1C] = {"INC E", 1, 4};
    lut[0x2C] = {"INC L", 1, 4};
    lut[0x3C] = {"INC A", 1, 4};
    
    /*
    Block 2
    add, adc, sub, sbc, and, xor, or, cp
    Two byte instructions
    */

    /*
    Block 3
    add, adc, sub, sbc, and, xor, or, cp
    ret, reti, jp, call, rst
    pop, push
    ldf
    ld
    ldh
    add sp, imm8
    ld hl, sp + imm8 
    ld sp, hl
    di
    ei
    0xCB take ntoe!!!!
    */
    lut[0xC3] = {"JP NZ, a16", 3, 12};
    lut[0xC4] = {"JP a16", 3, 12};
    lut[0xCA] = {"JP Z, a16", 3, 12};
    lut[0xDA] = {"JP C, a16", 3, 12};
    lut[0xD3] = {"JP NC, a16", 3, 12};
    lut[0xE9] = {"JP HL", 1, 4};

    lut[0xF3] = {"DI", 1, 4};
    lut[0xFB] = {"DI", 1, 4};

    return lut;
}

std::array<InstrInfo_t, 0x100> instr_lut = init_lut();