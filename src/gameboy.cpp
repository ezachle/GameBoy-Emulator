#include <format>
#include <filesystem>
#include "opcode.hpp"
#include "gameboy.hpp"

namespace fs = std::filesystem;

GameBoy::GameBoy(std::string file_name) {
    this->file_name = file_name;
    gb_file.open(file_name, std::ios::in | std::ios::binary);
    if(!gb_file.is_open()) {
        throw std::runtime_error(
            "Error: Invalid .gb file\n"
            "Usage: ./GameBoy <path to .GB>"
        );
    }

    gb_file_size = fs::file_size(fs::path(file_name));
    buffer = std::make_unique<uint8_t[]>(gb_file_size);
    if(!gb_file.read(reinterpret_cast<char*>(buffer.get()), gb_file_size)) {
        throw std::runtime_error(std::format(
            "Failed to read {} file", file_name
        ));
    }

    gb_file.close();
}

GameBoy::~GameBoy() {
}

void GameBoy::Run() {
    uint16_t *pc = &registers.pc;

    while(*pc < gb_file_size) {
        *pc += Disassemble(*pc);
    }
}

static bool carry(uint16_t a, uint16_t b, uint16_t result) {
    return ((a ^ b ^ result) & 0x100) != 0;
}

static bool half_carry(uint8_t a, uint8_t b, uint8_t result) {
    return ((a ^ b ^ result) & 0x10) != 0;
}

uint8_t GameBoy::Disassemble(uint16_t pc) {
    uint8_t opcode = buffer[pc];
    InstrInfo_t instr = instr_lut[opcode];
    Flags_t *flags = &registers.af.flags;
    uint16_t next_pc = false;

    switch(opcode) {
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
            {
                uint16_t *dst;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        dst = &registers.bc.raw;
                        break;
                    case 2:
                    case 3:
                        dst = &registers.de.raw;
                        break;
                    case 4:
                    case 5:
                        dst = &registers.hl.raw;
                        break;
                    case 6:
                    case 7:
                        dst = &registers.sp;
                        break;
                };
                
                *dst = GET_WORD(buffer, pc);
            }
            break;
        case 0x02:
        case 0x12:
        case 0x22:
        case 0x32:
            {
                uint16_t dst;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        dst = registers.bc.raw;
                        break;
                    case 2:
                    case 3:
                        dst = registers.de.raw;
                        break;
                    case 4:
                    case 5:
                        dst = registers.hl.raw;
                        registers.hl.raw++;
                        break;
                    case 6:
                    case 7:
                        dst = registers.hl.raw;
                        registers.hl.raw--;
                        break;
                };
                
                buffer[dst] = registers.af.a;
            }
            break;
        case 0x06:
        case 0x16:
        case 0x26:
        case 0x36:
            {
                uint8_t *dst;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        dst = &registers.bc.b;
                        break;
                    case 2:
                    case 3:
                        dst = &registers.de.d;
                        break;
                    case 4:
                    case 5:
                        dst = &registers.hl.h;
                        break;
                    case 6:
                    case 7:
                        dst = &buffer[registers.hl.raw];
                        break;
                };
                
                *dst = GET_BYTE(buffer, pc);
            }
            break;
        case 0x08:
            {
                uint16_t dst = GET_WORD(buffer, pc);
                buffer[dst] = registers.sp;
            }
            break;
        case 0x0A:
        case 0x1A:
        case 0x2A:
        case 0x3A:
            {
                uint16_t dst;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        dst = registers.bc.raw;
                        break;
                    case 2:
                    case 3:
                        dst = registers.de.raw;
                        break;
                    case 4:
                    case 5:
                        dst = registers.hl.raw;
                        registers.hl.raw++;
                        break;
                    case 6:
                    case 7:
                        dst = registers.hl.raw;
                        registers.hl.raw--;
                        break;
                };
                
                registers.af.a = buffer[dst];
            }
            break;
        case 0x0E:
        case 0x1E:
        case 0x2E:
        case 0x3E:
            {
                uint8_t *dst;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        dst = &registers.bc.c;
                        break;
                    case 2:
                    case 3:
                        dst = &registers.de.e;
                        break;
                    case 4:
                    case 5:
                        dst = &registers.hl.l;
                        break;
                    case 6:
                    case 7:
                        dst = &registers.af.a;
                        break;
                };

                *dst = GET_BYTE(buffer, pc);
            }
            break;
        case 0x03:
        case 0x13:
        case 0x23:
        case 0x33:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        reg = &registers.bc.raw;
                        break;
                    case 2:
                    case 3:
                        reg = &registers.de.raw;
                        break;
                    case 4:
                    case 5:
                        reg = &registers.hl.raw;
                        break;
                    case 6:
                    case 7:
                        reg = &registers.sp;
                        break;
                };

                reg++;
            }
            break;
        case 0x04:
        case 0x14:
        case 0x24:
        case 0x34:
        case 0x0C:
        case 0x1C:
        case 0x2C:
        case 0x3C:
            {
                uint8_t *reg = nullptr;
                switch(GET_DST(opcode)) {
                    case 0:
                        reg = &registers.bc.b;
                        break;
                    case 1:
                        reg = &registers.bc.c;
                        break;
                    case 2:
                        reg = &registers.de.d;
                        break;
                    case 3:
                        reg = &registers.de.e;
                        break;
                    case 4:
                        reg = &registers.hl.h;
                        break;
                    case 5:
                        reg = &registers.hl.l;
                        break;
                    case 6:
                        reg = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        reg = &registers.af.a;
                        break;
                };

                flags->h = half_carry(*reg, 1, *reg + 1);
                (*reg)++;
                flags->z = (reg == 0) ? 1 : 0;
                flags->n = 0;
            }
            break;
        case 0x0B:
        case 0x1B:
        case 0x2B:
        case 0x3B:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case 0:
                    case 1:
                        reg = &registers.bc.raw;
                        break;
                    case 2:
                    case 3:
                        reg = &registers.de.raw;
                        break;
                    case 4:
                    case 5:
                        reg = &registers.hl.raw;
                        break;
                    case 6:
                    case 7:
                        reg = &registers.sp;
                        break;
                };

                reg--;
            }
            break;
        case 0x05:
        case 0x15:
        case 0x25:
        case 0x35:
        case 0x0D:
        case 0x1D:
        case 0x2D:
        case 0x3D:
            {
                uint8_t *reg = nullptr;
                switch(GET_DST(opcode)) {
                    case 0:
                        reg = &registers.bc.b;
                        break;
                    case 1:
                        reg = &registers.bc.c;
                        break;
                    case 2:
                        reg = &registers.de.d;
                        break;
                    case 3:
                        reg = &registers.de.e;
                        break;
                    case 4:
                        reg = &registers.hl.h;
                        break;
                    case 5:
                        reg = &registers.hl.l;
                        break;
                    case 6:
                        reg = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        reg = &registers.af.a;
                        break;
                };

                flags->h = half_carry(*reg, 1, *reg - 1);
                (*reg)--;
                flags->z = (reg == 0) ? 1 : 0;
                flags->n = 1;
            }
            break;
        case 0x09:
        case 0x19:
        case 0x29:
        case 0x39:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case 1:
                        reg = &registers.bc.raw;
                        break;
                    case 3:
                        reg = &registers.hl.raw;
                        break;
                    case 5:
                        reg = &registers.hl.raw;
                        break;
                    case 7:
                        reg = &registers.sp;
                        break;
                }

                flags->c = carry(registers.hl.raw, *reg, registers.hl.raw + *reg);
                flags->h = half_carry(registers.hl.raw, *reg, registers.hl.raw + *reg);
                registers.hl.raw += *reg;
                flags->n = 0;
            }
            break;
        case 0xCD:
            {
                next_pc = GET_WORD(buffer, pc);

                registers.pc += instr.length;
                PUSH_SP(registers.pc, buffer, registers.sp);
            }
            break;
        case 0xC4:
            {
                if(!flags->z) {
                    next_pc = GET_WORD(buffer, pc);

                    registers.pc += instr.length;
                    PUSH_SP(registers.pc, buffer, registers.sp);

                    instr.cycles = 24;
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xCC:
            {
                if(flags->z) {
                    next_pc = GET_WORD(buffer, pc);

                    registers.pc += instr.length;
                    PUSH_SP(registers.pc, buffer, registers.sp);

                    instr.cycles = 24;
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xD4:
            {
                if(!flags->c) {
                    next_pc = GET_WORD(buffer, pc);

                    registers.pc += instr.length;
                    PUSH_SP(registers.pc, buffer, registers.sp);

                    instr.cycles = 24;
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xDC:
            {
                if(flags->c) {
                    next_pc = GET_WORD(buffer, pc);

                    registers.pc += instr.length;
                    PUSH_SP(registers.pc, buffer, registers.sp);

                    instr.cycles = 24;
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xC9:
            {
               POP_SP(next_pc, buffer, registers.sp); 
            }
            break;
        case 0xD9:
            {
               POP_SP(next_pc, buffer, registers.sp); 
               enable_interrupt = true;
            }
            break;
        case 0xC0:
            {
                if(!flags->z) {
                    POP_SP(next_pc, buffer, registers.sp); 
                    instr.cycles = 20;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0xC8:
            {
                if(flags->z) {
                    POP_SP(next_pc, buffer, registers.sp); 
                    instr.cycles = 20;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0xD0:
            {
                if(!flags->c) {
                    POP_SP(next_pc, buffer, registers.sp); 
                    instr.cycles = 20;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0xD8:
            {
                if(flags->c) {
                    POP_SP(next_pc, buffer, registers.sp); 
                    instr.cycles = 20;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0xC2:
            {
                if(!flags->z) {
                    instr.cycles = 16;
                    next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xC3:
            {
                next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
            }
            break;
        case 0xCA:
            {
                if(flags->z) {
                    instr.cycles = 16;
                    next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xDA:
            {
                if(flags->c) {
                    instr.cycles = 16;
                    next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xD2:
            {
                if(!flags->c) {
                    instr.cycles = 16;
                    next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xE9:
            {
                next_pc = (buffer[pc + 2] << 8) | buffer[pc + 1];
            }
            break;
        case 0xC1:
        case 0xD1:
        case 0xE1:
        case 0xF1:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case 0:
                        reg = &registers.bc.raw;
                        break;
                    case 2:
                        reg = &registers.de.raw;
                        break;
                    case 4:
                        reg = &registers.hl.raw;
                        break;
                    case 6:
                        reg = &registers.af.raw;
                        break;
                }

                POP_SP(*reg, buffer, registers.sp);
            }
            break;
        case 0xC5:
        case 0xD5:
        case 0xE5:
        case 0xF5:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case 0:
                        reg = &registers.bc.raw;
                        break;
                    case 2:
                        reg = &registers.de.raw;
                        break;
                    case 4:
                        reg = &registers.hl.raw;
                        break;
                    case 6:
                        reg = &registers.af.raw;
                        break;
                }

                PUSH_SP(*reg, buffer, registers.sp);
            }
            break;
        case 0xE0:
            {
                buffer[GET_BYTE(buffer, pc)] = registers.af.a;
            }
            break;
        case 0xE2:
            {
                buffer[registers.bc.c] = registers.af.a;
            }
            break;
        case 0xF0:
            {
                registers.af.a = buffer[GET_BYTE(buffer, pc)];
            }
            break;
        case 0xF2:
            {
                registers.af.a = buffer[registers.bc.c];
            }
            break;
        case 0xEA:
            {
                uint16_t addr = GET_WORD(buffer, pc);
                buffer[addr] = registers.af.a;
            }
            break;
        case 0xFA:
            {
                uint16_t addr = GET_WORD(buffer, pc);
                registers.af.a = addr;
            }
            break;
        case 0xE8:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                flags->c = carry(registers.sp, byte, registers.sp + byte);
                flags->h = half_carry(registers.sp, byte, registers.sp + byte);
                registers.sp += byte;

                flags->z = 0;
                flags->n = 0;
            }
            break;
        case 0xF8:
            {
                // TODO Double-check this
                int8_t signed_byte = static_cast<int8_t>(GET_BYTE(buffer, pc));
                flags->c = carry(registers.sp, signed_byte, registers.sp + signed_byte);
                flags->h = half_carry(registers.sp, signed_byte, registers.sp + signed_byte);
                registers.hl.raw = registers.sp + signed_byte;
                flags->z = 0;
                flags->n = 0;
            }
            break;
        case 0xF9:
            {
                registers.sp = registers.hl.raw;
            }
            break;
        case 0xF3:
            enable_interrupt = false;
            break;
        case 0xFB:
            enable_interrupt = true;
            break;
        case 0x00:
            break;
        default:
            std::cout << std::format("Unimplemented Instruction 0x{:02X}\n", opcode);
    }

    add_to_cycle(instr.cycles ? instr.cycles : 1);

#ifdef DEBUG
    std::cout << std::format("AF: 0x{:04X} BC: 0x{:04X} DE: 0x{:04X} HL: 0x{:04X} PC: 0x{:04X} SP: 0x{:04X} | {:02X} {:02X} {:02X} {:02X}", registers.af.raw, registers.bc.raw, registers.de.raw, registers.hl.raw, pc, registers.sp, buffer[pc], buffer[pc+1], buffer[pc+2], buffer[pc+3])<< std::endl;
    std::cout << std::format(" [AF]: 0x{:04X} [BC]: 0x{:04X} [DE]: 0x{:04X} [HL]: 0x{:04X}\n", buffer[registers.af.raw], buffer[registers.bc.raw], buffer[registers.de.raw], buffer[registers.hl.raw]) ;
    std::cout << std::format(" [a8]: 0x{:02X} [a16]: 0x{:02X}\n", buffer[GET_BYTE(buffer, pc)], buffer[GET_WORD(buffer, pc)]);
#endif
    if(next_pc) {
        set_pc(next_pc);
        return 0;
    } else {
        return instr.length ? instr.length : 1;
    }
}
