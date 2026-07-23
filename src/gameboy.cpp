#include <format>
#include <filesystem>
#include "opcode.hpp"
#include "gameboy.hpp"

namespace fs = std::filesystem;

GameBoy::GameBoy(std::string file_name) {
    file_name = file_name;
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

static bool half_carry(uint8_t a, uint8_t b, uint8_t cy) {
    uint16_t res = a + b + cy;
    return ((a ^ b ^ res) & 0x10) != 0;
}

uint8_t GameBoy::Disassemble(uint16_t pc) {
    uint8_t opcode = buffer[pc];
    InstrInfo_t instr = instr_lut[opcode];
    Flags_t *flags = &registers.af.flags;
    uint16_t jump_addr = false;

    std::cout << std::format("AF: 0x{:02X} BC: 0x{:02X} DE: 0x{:02X} HL: 0x{:02X} PC: 0x{:02X} | {:02X} {:02X} {:02X} {:02X}", registers.af.raw, registers.bc.raw, registers.de.raw, registers.hl.raw, pc, buffer[pc], buffer[pc+1], buffer[pc+2], buffer[pc+3])<< std::endl;
    switch(opcode) {
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
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
                        reg = &registers.sp;
                        break;
                };
                
                *reg = ((buffer[pc+2] << 8) | buffer[pc+1]);
            }
            break;
        case 0x02:
        case 0x12:
        case 0x22:
        case 0x32:
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
                        reg = &registers.sp;
                        break;
                };
                
                *reg = ((buffer[pc+2] << 8) | buffer[pc+1]);
            }
            break;
        case 0x06:
        case 0x16:
        case 0x26:
        case 0x36:
            {
                
            }
            break;
        case 0x08:
            {
                
            }
            break;
        case 0x0A:
        case 0x1A:
        case 0x2A:
        case 0x3A:
            {
                
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
                        reg = &registers.bc.raw;
                        break;
                    case 2:
                        reg = &registers.de.raw;
                        break;
                    case 4:
                        reg = &registers.hl.raw;
                        break;
                    case 6:
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

                (*reg)++;
                flags->z = (reg == 0) ? 1 : 0;
                flags->n = 0;
                flags->h = half_carry((*reg)-1, 1, flags->c);
            }
            break;
        case 0xC3:
            {
                if(!flags->z) {
                    instr.cycles = 16;
                    jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xC4:
            {
                jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
            }
            break;
        case 0xCA:
            {
                if(flags->z) {
                    instr.cycles = 16;
                    jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xDA:
            {
                if(flags->c) {
                    instr.cycles = 16;
                    jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xD3:
            {
                if(!flags->c) {
                    instr.cycles = 16;
                    jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
                } else {
                    instr.cycles = 12;
                }
            }
            break;
        case 0xE9:
            {
                jump_addr = (buffer[pc + 2] << 8) | buffer[pc + 1];
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

    if(jump_addr) {
        set_pc(jump_addr);
        return 0;
    } else {
        return instr.length ? instr.length : 1;
    }
}