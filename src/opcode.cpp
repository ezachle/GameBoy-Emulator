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

    lut[0x0B] = {"DEC BC", 1, 8};
    lut[0x1B] = {"DEC DE", 1, 8};
    lut[0x2B] = {"DEC HL", 1, 8};
    lut[0x3B] = {"DEC SP", 1, 8};

    lut[0x05] = {"DEC B", 1, 4};
    lut[0x15] = {"DEC D", 1, 4};
    lut[0x25] = {"DEC H", 1, 4};
    lut[0x35] = {"DEC [HL]", 1, 12};

    lut[0x0D] = {"DEC C", 1, 4};
    lut[0x1D] = {"DEC E", 1, 4};
    lut[0x2D] = {"DEC L", 1, 4};
    lut[0x3D] = {"DEC A", 1, 4};

    lut[0x09] = {"ADD HL, BC", 1, 8};
    lut[0x19] = {"ADD HL, DE", 1, 8};
    lut[0x29] = {"ADD HL, HL", 1, 8};
    lut[0x39] = {"ADD HL, SP", 1, 8};

    lut[0x18] = {"JR e8", 2, 12};
    lut[0x20] = {"JR NZ, e8", 2, 12};
    lut[0x28] = {"JR Z, e8", 2, 12};
    lut[0x30] = {"JR NC, e8", 2, 12};
    lut[0x38] = {"JR C, e8", 2, 12};
    
    // Block 1
    lut[0x40] = {"LD B, B", 1, 4};
    lut[0x41] = {"LD B, C", 1, 4};
    lut[0x42] = {"LD B, D", 1, 4};
    lut[0x43] = {"LD B, E", 1, 4};
    lut[0x44] = {"LD B, H", 1, 4};
    lut[0x45] = {"LD B, L", 1, 4};
    lut[0x46] = {"LD B, [HL]", 1, 8};
    lut[0x47] = {"LD B, A", 1, 4};
          
    lut[0x48] = {"LD C, B", 1, 4};
    lut[0x49] = {"LD C, C", 1, 4};
    lut[0x4A] = {"LD C, D", 1, 4};
    lut[0x4B] = {"LD C, E", 1, 4};
    lut[0x4C] = {"LD C, H", 1, 4};
    lut[0x4D] = {"LD C, L", 1, 4};
    lut[0x4E] = {"LD C, [HL]", 1, 8};
    lut[0x4F] = {"LD C, A", 1, 4};
          
    lut[0x50] = {"LD D, B", 1, 4};
    lut[0x51] = {"LD D, C", 1, 4};
    lut[0x52] = {"LD D, D", 1, 4};
    lut[0x53] = {"LD D, E", 1, 4};
    lut[0x54] = {"LD D, H", 1, 4};
    lut[0x55] = {"LD D, L", 1, 4};
    lut[0x56] = {"LD D, [HL]", 1, 8};
    lut[0x57] = {"LD D, A", 1, 4};
          
    lut[0x58] = {"LD E, B", 1, 4};
    lut[0x59] = {"LD E, C", 1, 4};
    lut[0x5A] = {"LD E, D", 1, 4};
    lut[0x5B] = {"LD E, E", 1, 4};
    lut[0x5C] = {"LD E, H", 1, 4};
    lut[0x5D] = {"LD E, L", 1, 4};
    lut[0x5E] = {"LD E, [HL]", 1, 8};
    lut[0x5F] = {"LD E, A", 1, 4};
          
    lut[0x60] = {"LD H, B", 1, 4};
    lut[0x61] = {"LD H, C", 1, 4};
    lut[0x62] = {"LD H, D", 1, 4};
    lut[0x63] = {"LD H, E", 1, 4};
    lut[0x64] = {"LD H, H", 1, 4};
    lut[0x65] = {"LD H, L", 1, 4};
    lut[0x66] = {"LD H, [HL]", 1, 8};
    lut[0x67] = {"LD H, A", 1, 4};
          
    lut[0x68] = {"LD L, B", 1, 4};
    lut[0x69] = {"LD L, C", 1, 4};
    lut[0x6A] = {"LD L, D", 1, 4};
    lut[0x6B] = {"LD L, E", 1, 4};
    lut[0x6C] = {"LD L, H", 1, 4};
    lut[0x6D] = {"LD L, L", 1, 4};
    lut[0x6E] = {"LD L, [HL]", 1, 8};
    lut[0x6F] = {"LD L, A", 1, 4};
          
    lut[0x70] = {"LD [HL], B", 1, 8};
    lut[0x71] = {"LD [HL], C", 1, 8};
    lut[0x72] = {"LD [HL], D", 1, 8};
    lut[0x73] = {"LD [HL], E", 1, 8};
    lut[0x74] = {"LD [HL], H", 1, 8};
    lut[0x75] = {"LD [HL], L", 1, 8};
    //lut[0x76] = {"HALT", 1, 4};
    lut[0x77] = {"LD [HL], A", 1, 8};

    lut[0x78] = {"LD A, B", 1, 4};
    lut[0x79] = {"LD A, C", 1, 4};
    lut[0x7A] = {"LD A, D", 1, 4};
    lut[0x7B] = {"LD A, E", 1, 4};
    lut[0x7C] = {"LD A, H", 1, 4};
    lut[0x7D] = {"LD A, L", 1, 4};
    lut[0x7E] = {"LD A, [HL]", 1, 8};
    lut[0x7F] = {"LD A, A", 1, 4};

    /*
    Block 2
    add, adc, sub, sbc, and, xor, or, cp
    Two byte instructions
    */
    lut[0x80] = {"ADD A,B", 1, 4};
    lut[0x81] = {"ADD A,C", 1, 4};
    lut[0x82] = {"ADD A,D", 1, 4};
    lut[0x83] = {"ADD A,E", 1, 4};
    lut[0x84] = {"ADD A,H", 1, 4};
    lut[0x85] = {"ADD A,L", 1, 4};
    lut[0x86] = {"ADD A,[HL]", 1, 8};
    lut[0x87] = {"ADD A,A", 1, 4};

    lut[0x88] = {"ADC A,B", 1, 4};
    lut[0x89] = {"ADC A,C", 1, 4};
    lut[0x8A] = {"ADC A,D", 1, 4};
    lut[0x8B] = {"ADC A,E", 1, 4};
    lut[0x8C] = {"ADC A,H", 1, 4};
    lut[0x8D] = {"ADC A,L", 1, 4};
    lut[0x8E] = {"ADC A,[HL]", 1, 8};
    lut[0x8F] = {"ADC A,A", 1, 4};

    lut[0x90] = {"SUB A,B", 1, 4};
    lut[0x91] = {"SUB A,C", 1, 4};
    lut[0x92] = {"SUB A,D", 1, 4};
    lut[0x93] = {"SUB A,E", 1, 4};
    lut[0x94] = {"SUB A,H", 1, 4};
    lut[0x95] = {"SUB A,L", 1, 4};
    lut[0x96] = {"SUB A,[HL]", 1, 8};
    lut[0x97] = {"SUB A,A", 1, 4};

    lut[0x98] = {"SBC A,B", 1, 4};
    lut[0x99] = {"SBC A,C", 1, 4};
    lut[0x9A] = {"SBC A,D", 1, 4};
    lut[0x9B] = {"SBC A,E", 1, 4};
    lut[0x9C] = {"SBC A,H", 1, 4};
    lut[0x9D] = {"SBC A,L", 1, 4};
    lut[0x9E] = {"SBC A,[HL]", 1, 8};
    lut[0x9F] = {"SBC A,A", 1, 4};

    lut[0xA0] = {"AND A,B", 1, 4};
    lut[0xA1] = {"AND A,C", 1, 4};
    lut[0xA2] = {"AND A,D", 1, 4};
    lut[0xA3] = {"AND A,E", 1, 4};
    lut[0xA4] = {"AND A,H", 1, 4};
    lut[0xA5] = {"AND A,L", 1, 4};
    lut[0xA6] = {"AND A,[HL]", 1, 8};
    lut[0xA7] = {"AND A,A", 1, 4};

    lut[0xA8] = {"XOR A,B", 1, 4};
    lut[0xA9] = {"XOR A,C", 1, 4};
    lut[0xAA] = {"XOR A,D", 1, 4};
    lut[0xAB] = {"XOR A,E", 1, 4};
    lut[0xAC] = {"XOR A,H", 1, 4};
    lut[0xAD] = {"XOR A,L", 1, 4};
    lut[0xAE] = {"XOR A,[HL]", 1, 8};
    lut[0xAF] = {"XOR A,A", 1, 4};

    lut[0xB0] = {"OR A,B", 1, 4};
    lut[0xB1] = {"OR A,C", 1, 4};
    lut[0xB2] = {"OR A,D", 1, 4};
    lut[0xB3] = {"OR A,E", 1, 4};
    lut[0xB4] = {"OR A,H", 1, 4};
    lut[0xB5] = {"OR A,L", 1, 4};
    lut[0xB6] = {"OR A,[HL]", 1, 8};
    lut[0xB7] = {"OR A,A", 1, 4};

    lut[0xB8] = {"CP A,B", 1, 4};
    lut[0xB9] = {"CP A,C", 1, 4};
    lut[0xBA] = {"CP A,D", 1, 4};
    lut[0xBB] = {"CP A,E", 1, 4};
    lut[0xBC] = {"CP A,H", 1, 4};
    lut[0xBD] = {"CP A,L", 1, 4};
    lut[0xBE] = {"CP A,[HL]", 1, 8};
    lut[0xBF] = {"CP A,A", 1, 4};

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
    lut[0xCD] = {"CALL a16", 3, 24};

    lut[0xC4] = {"CALL NZ, a16", 3, 24};
    lut[0xCC] = {"CALL Z, a16", 3, 24};

    lut[0xD4] = {"CALL NC, a16", 3, 24};
    lut[0xDC] = {"CALL C, a16", 3, 24};

    lut[0xC9] = {"RET", 1, 16};
    lut[0xD9] = {"RETI", 1, 16};
    lut[0xC0] = {"RET NZ", 1, 20};
    lut[0xC8] = {"RET Z", 1, 20};
    lut[0xD0] = {"RET NC", 1, 20};
    lut[0xD8] = {"RET C", 1, 20};

    lut[0xC2] = {"JP NZ, a16", 3, 12};
    lut[0xC3] = {"JP a16", 3, 12};
    lut[0xCA] = {"JP Z, a16", 3, 12};
    lut[0xDA] = {"JP C, a16", 3, 12};
    lut[0xD2] = {"JP NC, a16", 3, 12};
    lut[0xE9] = {"JP HL", 1, 4};

    lut[0xC1] = {"POP BC", 1, 12};
    lut[0xD1] = {"POP DE", 1, 12};
    lut[0xE1] = {"POP HL", 1, 12};
    lut[0xF1] = {"POP AF", 1, 12};

    lut[0xC5] = {"PUSH BC", 1, 16};
    lut[0xD5] = {"PUSH DE", 1, 16};
    lut[0xE5] = {"PUSH HL", 1, 16};
    lut[0xF5] = {"PUSH AF", 1, 16};

    lut[0xE0] = {"LDH [a8], A", 2, 12};
    lut[0xE2] = {"LDH [C], A", 1, 8};
    lut[0xF0] = {"LDH A, [a8]", 2, 12};
    lut[0xF2] = {"LDH A, [C]", 1, 8};

    lut[0xEA] = {"LD [a16], A", 3, 16};
    lut[0xFA] = {"LD A, [a16]", 3, 16};

    lut[0xC6] = {"ADD A, n8", 2, 8};
    lut[0xD6] = {"SUB A, n8", 2, 8};
    lut[0xE6] = {"AND A, n8", 2, 8};
    lut[0xF6] = {"OR A, n8", 2, 8};

    lut[0xCE] = {"ADC A, n8", 2, 8};
    lut[0xDE] = {"SBC A, n8", 2, 8};
    lut[0xEE] = {"XOR A, n8", 2, 8};
    lut[0xFE] = {"CP A, n8", 2, 8};

    lut[0xE8] = {"ADD SP, e8", 2, 16};
    lut[0xF8] = {"LD HL, SP+e8", 2, 12};
    lut[0xF9] = {"LD SP, HL", 1, 8};

    lut[0xF3] = {"DI", 1, 4};
    lut[0xFB] = {"EI", 1, 4};

    return lut;
}

std::array<InstrInfo_t, 0x100> instr_lut = init_lut();
