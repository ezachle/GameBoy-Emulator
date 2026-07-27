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

uint8_t GameBoy::Disassemble(uint16_t pc) {
    uint8_t opcode = buffer[pc];
    InstrInfo_t instr = instr_lut[opcode];
    Flags_t *flags = &registers.af.f;
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

                flags->h = ((*reg & 0x0F) + (1)) > 0x0F;
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

                flags->h = (*reg == 0) ? 1 : 0;
                (*reg)--;
                flags->z = (*reg == 0) ? 1 : 0;
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

                flags->h = ((registers.hl.raw & 0x0F) + (*reg & 0x0F)) > 0x0F;
                flags->c = ((registers.hl.raw & 0xFF) + (*reg & 0xFF)) > 0xFF;
                registers.hl.raw += *reg;
                flags->n = 0;
            }
            break;
        case 0x18:
            {
                uint8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                next_pc = pc + instr.length + relative_pc;
            }
            break;
        case 0x20:
            {
                uint8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(!flags->z) {
                    next_pc = pc + instr.length + relative_pc;
                    instr.cycles = 12;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0x28:
            {
                uint8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(flags->z) {
                    next_pc = pc + instr.length + relative_pc;
                    instr.cycles = 12;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0x30:
            {
                uint8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(!flags->c) {
                    next_pc = pc + instr.length + relative_pc;
                    instr.cycles = 12;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0x38:
            {
                uint8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(flags->c) {
                    next_pc = pc + instr.length + relative_pc;
                    instr.cycles = 12;
                } else {
                    instr.cycles = 8;
                }
            }
            break;
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F:
            {
                uint8_t *dst;
                uint8_t *src;
                switch(GET_DST(opcode)) {
                    case 0:
                        dst = &registers.bc.b;
                        break;
                    case 1:
                        dst = &registers.bc.c;
                        break;
                    case 2:
                        dst = &registers.de.d;
                        break;
                    case 3:
                        dst = &registers.de.e;
                        break;
                    case 4:
                        dst = &registers.hl.h;
                        break;
                    case 5:
                        dst = &registers.hl.l;
                        break;
                    case 6:
                        dst = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        dst = &registers.af.a;
                        break;
                };

                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                *dst = *src;
            }
            break;
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                flags->h = ((registers.af.a & 0x0F) + (*src & 0x0F)) > 0x0F;
                flags->c = ((registers.af.a & 0xFF) + (*src & 0xFF)) > 0xFF;
                flags->z = ((registers.af.a + *src) == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a += *src;
            }
            break;
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                flags->h = ((registers.af.a & 0x0F) + (*src & 0x0F) + (flags->c)) > 0x0F;
                flags->c = ((registers.af.a & 0xFF) + (*src & 0xFF) + (flags->c)) > 0xFF;
                flags->z = ((registers.af.a + *src) == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a += *src + flags->c;
            }
            break;
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                flags->h = ((registers.af.a & 0x0F) < (*src & 0x0F));
                flags->c = registers.af.a < *src;
                flags->z = ((registers.af.a - *src) == 0) ? 1 : 0;
                flags->n = 1;

                registers.af.a -= *src;
            }
            break;
        case 0x97:
            {
                flags->z = 1;
                flags->n = 1;
                flags->h = 0;
                flags->c = 0;
                registers.af.a -= registers.af.a;
            }
            break;
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                uint16_t result = registers.af.a - *src + flags->c;
                flags->h = ((registers.af.a & 0x0F) - (*src & 0x0F) - flags->c) < 0;
                flags->c = result < 0;
                flags->z = ((result & 0xFF) == 0) ? 1 : 0;
                flags->n = 1;

                registers.af.a = result;
            }
            break;
        case 0x9F:
            {
                uint16_t result = registers.af.a - registers.af.a + flags->c;
                flags->z = ((result & 0xFF) == 0) ? 1 : 0;
                flags->n = 1;
                flags->h = ((registers.af.a & 0x0F) - (registers.af.a & 0x0F) - flags->c) < 0;
                registers.af.a = result;
            }
            break;
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                uint8_t result = registers.af.a & (*src);
                flags->h = 1;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a = result;
            }
            break;
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                uint8_t result = registers.af.a ^ (*src);
                flags->h = 0;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a = result;
            }
            break;
        case 0xAF:
            {
                flags->z = 1;
                flags->n = 0;
                flags->h = 0;
                flags->c = 0;

                registers.af.a ^= registers.af.a;
            }
            break;
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                uint8_t result = registers.af.a | (*src);
                flags->h = 0;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a = result;
            }
            break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
            {
                uint8_t *src;
                switch(GET_SRC(opcode)) {
                    case 0:
                        src = &registers.bc.b;
                        break;
                    case 1:
                        src = &registers.bc.c;
                        break;
                    case 2:
                        src = &registers.de.d;
                        break;
                    case 3:
                        src = &registers.de.e;
                        break;
                    case 4:
                        src = &registers.hl.h;
                        break;
                    case 5:
                        src = &registers.hl.l;
                        break;
                    case 6:
                        src = &buffer[registers.hl.raw];
                        break;
                    case 7:
                        src = &registers.af.a;
                        break;
                };

                flags->h = ((registers.af.a & 0x0F) < (*src & 0x0F));
                flags->c = registers.af.a < *src;
                flags->z = ((registers.af.a - *src) == 0) ? 1 : 0;
                flags->n = 1;
            }
            break;
        case 0xBF:
            {
                flags->z = 1;
                flags->n = 1;
                flags->h = 0;
                flags->c = 0;
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
        case 0xC6:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                uint16_t result = registers.af.a + byte;
                flags->z = (result == 0);
                flags->n = 0;
                flags->h = ((registers.af.a & 0x0F) + (byte & 0x0F)) > 0xF;
                flags->c = (result > 0xFF);
                registers.af.a = result;
            }
            break;
        case 0xD6:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                flags->h = ((registers.af.a & 0x0F) < (byte & 0x0F));
                flags->c = registers.af.a < byte;
                flags->z = ((registers.af.a - byte) == 0) ? 1 : 0;
                flags->n = 1;
                registers.af.a -= byte;
            }
            break;
        case 0xE6:
            {
                uint8_t result = registers.af.a & GET_BYTE(buffer, pc);
                flags->h = 1;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a = result;
            }
            break;
        case 0xF6:
            {
                uint8_t result = registers.af.a | GET_BYTE(buffer, pc);
                flags->h = 0;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;
                registers.af.a = result;
            }
            break;
        case 0xCE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                flags->h = ((registers.af.a & 0x0F) + (byte & 0x0F) + (flags->c)) > 0x0F;
                flags->c = ((registers.af.a & 0xFF) + (byte & 0xFF) + (flags->c)) > 0xFF;
                flags->z = ((registers.af.a + byte) == 0) ? 1 : 0;
                flags->n = 0;
                registers.af.a += byte + flags->c;
            }
            break;
        case 0xDE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                uint16_t result = registers.af.a - byte + flags->c;
                flags->h = ((registers.af.a & 0x0F) - (byte & 0x0F) - flags->c) < 0;
                flags->c = result < 0;
                flags->z = ((result & 0xFF) == 0) ? 1 : 0;
                flags->n = 1;

                registers.af.a = result;
            }
            break;
        case 0xEE:
            {
                uint8_t result = registers.af.a ^ GET_BYTE(buffer, pc);
                flags->h = 0;
                flags->c = 0;
                flags->z = (result == 0) ? 1 : 0;
                flags->n = 0;

                registers.af.a = result;
            }
            break;
        case 0xFE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                flags->h = ((registers.af.a & 0x0F) < (byte & 0x0F));
                flags->c = registers.af.a < byte;
                flags->z = ((registers.af.a - byte) == 0) ? 1 : 0;
                flags->n = 1;
            }
            break;
        case 0xE8:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                flags->c = ((registers.sp & 0xFF) + (byte & 0xFF)) > 0xFF;
                flags->h = ((registers.sp & 0x0F) + (byte & 0x0F)) > 0x0F;
                registers.sp += byte;

                flags->z = 0;
                flags->n = 0;
            }
            break;
        case 0xF8:
            {
                int8_t signed_byte = static_cast<int8_t>(GET_BYTE(buffer, pc));
                flags->c = ((registers.sp & 0xFF) + (signed_byte & 0xFF)) > 0xFF;
                flags->h = ((registers.sp & 0x0F) + (signed_byte & 0x0F)) > 0x0F;
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
    std::cout << std::format("Zero: {} Subtraction {} Half Carry {} Carry {}\n", ZERO_FLAG(registers.af.f.raw), SUB_FLAG(registers.af.f.raw), HALF_CARRY_FLAG(registers.af.f.raw), CARRY_FLAG(registers.af.f.raw));
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
