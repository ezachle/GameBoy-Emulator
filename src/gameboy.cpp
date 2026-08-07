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
    buffer = std::make_unique<uint8_t[]>(0x10000);
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
    while(registers.pc < 0xFFFF) {
        Disassemble();
    }
}

uint8_t* GameBoy::get_src_reg_u8(uint8_t opcode) {
    switch(GET_SRC(opcode)) {
        case REG_B:
            return &registers.b;
        case REG_C:
            return &registers.c;
        case REG_D:
            return &registers.d;
        case REG_E:
            return &registers.e;
        case REG_H:
            return &registers.h;
        case REG_L:
            return &registers.l;
        case REG_X:
            return &buffer[registers.hl];
        case REG_A:
            return &registers.a;
    };

    return nullptr;
}

uint8_t* GameBoy::get_dst_reg_u8(uint8_t opcode) {
    switch(GET_DST(opcode)) {
        case REG_B:
            return &registers.b;
        case REG_C:
            return &registers.c;
        case REG_D:
            return &registers.d;
        case REG_E:
            return &registers.e;
        case REG_H:
            return &registers.h;
        case REG_L:
            return &registers.l;
        case REG_X:
            return &buffer[registers.hl];
        case REG_A:
            return &registers.a;
    };

    return nullptr;
}

#define CALL(new_pc)                                                \
    jumping = true;                                                 \
    PUSH_SP((registers.pc + instr->length), buffer, registers.sp);   \
    registers.pc = new_pc;                                          \

void GameBoy::memory_write(uint16_t addr, uint8_t data) {
    buffer[addr] = data;

    if(addr == 0xFF02 && (data == 0x81 || data == 0x80)) {
        uint8_t c = buffer[0xFF01];
        putchar(c);
        fflush(stdout);
        buffer[0xFF02] = 0x01;
    }
}

uint8_t GameBoy::memory_read(uint16_t addr) {
    // temporary return - 0xFF44 refers to the scanline registers
    // tests expect this to return 0x90
    if(addr == 0xFF44) return 0x90;
    return buffer[addr];
}

void GameBoy::process_instructions(uint16_t pc, InstrInfo_t *instr) {
    uint8_t opcode = memory_read(pc);

    switch(opcode) {
        case 0x01:
        case 0x11:
        case 0x21:
        case 0x31:
            {
                uint16_t *dst;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        dst = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        dst = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        dst = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
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
                    case REG_B:
                    case REG_C:
                        dst = registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        dst = registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        dst = registers.hl;
                        registers.hl++;
                        break;
                    case REG_X:
                    case REG_A:
                        dst = registers.hl;
                        registers.hl--;
                        break;
                };
                
                memory_write(dst, registers.a);
            }
            break;
        case 0x06:
        case 0x16:
        case 0x26:
        case 0x36:
            {
                uint8_t *dst = nullptr;
                uint8_t data = GET_BYTE(buffer, pc);
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        dst = &registers.b;
                        break;
                    case REG_D:
                    case REG_E:
                        dst = &registers.d;
                        break;
                    case REG_H:
                    case REG_L:
                        dst = &registers.h;
                        break;
                    case REG_X:
                    case REG_A:
                        memory_write(registers.hl, data);
                        break;
                };
                
                if(dst != nullptr) {
                    *dst = data;
                }
            }
            break;
        case 0x08:
            {
                uint16_t dst = GET_WORD(buffer, pc);
                memory_write(dst, registers.sp & 0xFF);
                memory_write(dst+1, (registers.sp >> 8) & 0xFF);
            }
            break;
        case 0x0A:
        case 0x1A:
        case 0x2A:
        case 0x3A:
            {
                uint16_t dst;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        dst = registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        dst = registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        dst = registers.hl;
                        registers.hl++;
                        break;
                    case REG_X:
                    case REG_A:
                        dst = registers.hl;
                        registers.hl--;
                        break;
                };
                
                registers.a = memory_read(dst);
            }
            break;
        case 0x0E:
        case 0x1E:
        case 0x2E:
        case 0x3E:
            {
                uint8_t *dst;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        dst = &registers.c;
                        break;
                    case REG_D:
                    case REG_E:
                        dst = &registers.e;
                        break;
                    case REG_H:
                    case REG_L:
                        dst = &registers.l;
                        break;
                    case REG_X:
                    case REG_A:
                        dst = &registers.a;
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
                    case REG_B:
                    case REG_C:
                        reg = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        reg = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        reg = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
                        reg = &registers.sp;
                        break;
                };

                (*reg)++;
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
                uint8_t *reg = get_dst_reg_u8(opcode);
                uint8_t result = *reg + 1;

                set_flag(result == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((*reg & 0x0F) + (1 & 0x0F)) > 0x0F, HALF_CARRY_FLAG);

                *reg = result;
            }
            break;
        case 0x0B:
        case 0x1B:
        case 0x2B:
        case 0x3B:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        reg = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        reg = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        reg = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
                        reg = &registers.sp;
                        break;
                };

                (*reg)--;
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
                uint8_t *reg = get_dst_reg_u8(opcode);
                uint8_t result = *reg - 1;

                set_flag(result == 0, ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(((*reg & 0x0F) - (1 & 0x0F)) < 0, HALF_CARRY_FLAG);

                *reg = result;
            }
            break;
        case 0x09:
        case 0x19:
        case 0x29:
        case 0x39:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        reg = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        reg = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        reg = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
                        reg = &registers.sp;
                        break;
                };

                uint32_t result = *reg + registers.hl;
                set_flag(0, SUB_FLAG);
                set_flag(((registers.hl & 0xFFF) + (*reg & 0xFFF)) > 0xFFF, HALF_CARRY_FLAG);
                set_flag(result > 0xFFFF, CARRY_FLAG);

                registers.hl = result;
            }
            break;
        case 0x18:
            {
                int8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                registers.pc = pc + instr->length + relative_pc;
                jumping = true;
            }
            break;
        case 0x20:
            {
                int8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(!get_flag(ZERO_FLAG)) {
                    registers.pc = pc + instr->length + relative_pc;
                    jumping = true;
                    instr->cycles = 12;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0x28:
            {
                int8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(get_flag(ZERO_FLAG)) {
                    registers.pc = pc + instr->length + relative_pc;
                    jumping = true;
                    instr->cycles = 12;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0x30:
            {
                int8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(!get_flag(CARRY_FLAG)) {
                    registers.pc = pc + instr->length + relative_pc;
                    jumping = true;
                    instr->cycles = 12;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0x38:
            {
                int8_t relative_pc = static_cast<int8_t>(GET_BYTE(buffer, pc));
                if(get_flag(CARRY_FLAG)) {
                    registers.pc = pc + instr->length + relative_pc;
                    jumping = true;
                    instr->cycles = 12;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0x07:
            {
                uint8_t b7 = (registers.a >> 7);
                registers.a = (registers.a << 1) | b7;

                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b7, CARRY_FLAG);
            }
            break;
        case 0x17:
            {
                uint8_t carry = get_flag(CARRY_FLAG);
                uint8_t b7 = (registers.a >> 7);
                registers.a = (registers.a << 1) | carry;

                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b7, CARRY_FLAG);
            }
            break;
        case 0x0F:
            {
                uint8_t b0 = registers.a & 1; 
                registers.a = (registers.a >> 1) | (b0 << 7);

                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
            }
            break;
        case 0x1F:
            {
                uint8_t carry = get_flag(CARRY_FLAG);
                uint8_t b0 = registers.a & 1;
                registers.a = (registers.a >> 1) | (carry << 7);

                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
            }
            break;
        case 0x27:
            {
                // needs to be tested
                uint8_t old_hf = get_flag(HALF_CARRY_FLAG);
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint16_t offset = 0;
                if((get_flag(SUB_FLAG) == 0 && (registers.a & 0x0F) > 0x9) || old_hf) {
                    offset |= 0x6;
                }

                if((get_flag(SUB_FLAG) == 0 && (registers.a & 0xFF) > 0x99) || old_cf) {
                    offset |= 0x60;
                    set_flag(1, CARRY_FLAG);
                }

                if(get_flag(SUB_FLAG) == 0)
                    registers.a += offset;
                else
                    registers.a -= offset;

                set_flag((registers.a == 0), ZERO_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
            }
            break;
        case 0x37:
            {
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(1, CARRY_FLAG);
            }
            break;
        case 0x2F:
            {
                registers.a = ~registers.a;
                set_flag(1, SUB_FLAG);
                set_flag(1, HALF_CARRY_FLAG);
            }
            break;
        case 0x3F:
            {
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(get_flag(CARRY_FLAG) == 0, CARRY_FLAG);
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
                uint8_t *dst = get_dst_reg_u8(opcode);
                uint8_t *src = get_src_reg_u8(opcode);

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
                uint8_t *src = get_src_reg_u8(opcode);
                uint16_t result = registers.a + *src;

                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.a & 0x0F) + (*src & 0x0F)) > 0x0F, HALF_CARRY_FLAG);
                set_flag(result > 0xFF, CARRY_FLAG);

                registers.a = result;
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
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint16_t result = registers.a + *src + old_cf;

                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.a & 0x0F) + (*src & 0x0F) + old_cf) > 0x0F, HALF_CARRY_FLAG);
                set_flag(result > 0xFF, CARRY_FLAG);

                registers.a = result;
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
                uint8_t *src = get_src_reg_u8(opcode);

                set_flag((registers.a - *src) == 0 , ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag((registers.a & 0x0F) < (*src & 0x0F), HALF_CARRY_FLAG);
                set_flag(registers.a < *src, CARRY_FLAG);

                registers.a -= *src;
            }
            break;
        case 0x97:
            {
                set_flag(1 , ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);
                registers.a -= registers.a;
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
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint16_t result = registers.a - *src - old_cf;

                set_flag((result & 0xFF) == 0 , ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(((registers.a & 0x0F) - (*src & 0x0F) - old_cf) < 0, HALF_CARRY_FLAG);
                set_flag(result > 0xFF, CARRY_FLAG);

                registers.a = static_cast<uint8_t>(result);
            }
            break;
        case 0x9F:
            {
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint16_t result = registers.a - registers.a - old_cf;

                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(((registers.a & 0x0F) - (registers.a & 0x0F) - old_cf) < 0, HALF_CARRY_FLAG);

                registers.a = static_cast<uint8_t>(result);
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
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t result = registers.a & (*src);

                set_flag(result == 0 , ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(1, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a = result;
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
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t result = registers.a ^ (*src);

                set_flag(result == 0 , ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a = result;
            }
            break;
        case 0xAF:
            {
                set_flag(1 , ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a ^= registers.a;
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
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t result = registers.a | (*src);
                set_flag(result == 0 , ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a = result;
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
                uint8_t *src = get_src_reg_u8(opcode);
                set_flag((registers.a - *src) == 0 , ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(((registers.a & 0x0F) < (*src & 0x0F)), HALF_CARRY_FLAG);
                set_flag(registers.a < *src, CARRY_FLAG);
            }
            break;
        case 0xBF:
            {
                set_flag(1, ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);
            }
            break;
        case 0xCD:
            {
                CALL(GET_WORD(buffer, pc));
            }
            break;
        case 0xC4:
            {
                if(!get_flag(ZERO_FLAG)) {
                    CALL(GET_WORD(buffer, pc));
                    instr->cycles = 24;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xCC:
            {
                if(get_flag(ZERO_FLAG)) {
                    CALL(GET_WORD(buffer, pc));
                    instr->cycles = 24;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xD4:
            {
                if(!get_flag(CARRY_FLAG)) {
                    CALL(GET_WORD(buffer, pc));
                    instr->cycles = 24;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xDC:
            {
                if(get_flag(CARRY_FLAG)) {
                    CALL(GET_WORD(buffer, pc));
                    instr->cycles = 24;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xC9:
            {
                POP_SP(registers.pc, buffer, registers.sp); 
                jumping = true;
            }
            break;
        case 0xD9:
            {
                POP_SP(registers.pc, buffer, registers.sp); 
                jumping = true;
                enable_interrupt = true;
            }
            break;
        case 0xC0:
            {
                if(!get_flag(ZERO_FLAG)) {
                    POP_SP(registers.pc, buffer, registers.sp); 
                    jumping = true;
                    instr->cycles = 20;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0xC8:
            {
                if(get_flag(ZERO_FLAG)) {
                    POP_SP(registers.pc, buffer, registers.sp); 
                    jumping = true;
                    instr->cycles = 20;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0xD0:
            {
                if(!get_flag(CARRY_FLAG)) {
                    POP_SP(registers.pc, buffer, registers.sp); 
                    jumping = true;
                    instr->cycles = 20;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0xD8:
            {
                if(get_flag(CARRY_FLAG)) {
                    POP_SP(registers.pc, buffer, registers.sp); 
                    jumping = true;
                    instr->cycles = 20;
                } else {
                    instr->cycles = 8;
                }
            }
            break;
        case 0xC2:
            {
                if(!get_flag(ZERO_FLAG)) {
                    instr->cycles = 16;
                    registers.pc = GET_WORD(buffer, pc);
                    jumping = true;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xC3:
            {
                registers.pc = GET_WORD(buffer, pc);
                jumping = true;
            }
            break;
        case 0xCA:
            {
                if(get_flag(ZERO_FLAG)) {
                    instr->cycles = 16;
                    jumping = true;
                    registers.pc = GET_WORD(buffer, pc);
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xDA:
            {
                if(get_flag(CARRY_FLAG)) {
                    instr->cycles = 16;
                    registers.pc = GET_WORD(buffer, pc);
                    jumping = true;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xD2:
            {
                if(!get_flag(CARRY_FLAG)) {
                    instr->cycles = 16;
                    registers.pc = GET_WORD(buffer, pc);
                    jumping = true;
                } else {
                    instr->cycles = 12;
                }
            }
            break;
        case 0xE9:
            {
                registers.pc = registers.hl;
                jumping = true;
            }
            break;
        case 0xC1:
        case 0xD1:
        case 0xE1:
        case 0xF1:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        reg = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        reg = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        reg = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
                        reg = &registers.af;
                        break;
                };

                POP_SP(*reg, buffer, registers.sp);

                if(reg == &registers.af)
                   *reg &= 0xFFF0;
            }
            break;
        case 0xC5:
        case 0xD5:
        case 0xE5:
        case 0xF5:
            {
                uint16_t *reg;
                switch(GET_DST(opcode)) {
                    case REG_B:
                    case REG_C:
                        reg = &registers.bc;
                        break;
                    case REG_D:
                    case REG_E:
                        reg = &registers.de;
                        break;
                    case REG_H:
                    case REG_L:
                        reg = &registers.hl;
                        break;
                    case REG_X:
                    case REG_A:
                        reg = &registers.af;
                        break;
                };

                if(reg == &registers.af)
                    *reg &= 0xFFF0;

                PUSH_SP(*reg, buffer, registers.sp);
            }
            break;
        case 0xE0:
            {
                uint8_t offset = memory_read(pc+1);
                memory_write(0xFF00 + offset, registers.a);
            }
            break;
        case 0xE2:
            {
                memory_write(0xFF00 + registers.c, registers.a);
            }
            break;
        case 0xF0:
            {
                uint8_t offset = memory_read(pc + 1);
                registers.a = memory_read(0xFF00 + offset);
            }
            break;
        case 0xF2:
            {
                registers.a = memory_read(0xFF00 + registers.c);
            }
            break;
        case 0xEA:
            {
                uint16_t addr = GET_WORD(buffer, pc);
                memory_write(addr, registers.a);
            }
            break;
        case 0xFA:
            {
                uint16_t addr = GET_WORD(buffer, pc);
                registers.a = memory_read(addr);
            }
            break;
        case 0xC6:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                uint16_t result = registers.a + byte;
                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.a & 0x0F) + (byte & 0x0F)) > 0xF, HALF_CARRY_FLAG);
                set_flag(result > 0xFF, CARRY_FLAG);
                registers.a = result;
            }
            break;
        case 0xD6:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                set_flag(((registers.a - byte) == 0), ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag((registers.a & 0x0F) < (byte & 0x0F), HALF_CARRY_FLAG);
                set_flag(registers.a < byte, CARRY_FLAG);
                registers.a -= byte;
            }
            break;
        case 0xE6:
            {
                uint8_t result = registers.a & GET_BYTE(buffer, pc);
                set_flag(result == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(1, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a = result;
            }
            break;
        case 0xF6:
            {
                uint8_t result = registers.a | GET_BYTE(buffer, pc);
                set_flag(result == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);
                registers.a = result;
            }
            break;
        case 0xCE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint16_t result = registers.a + byte + old_cf;

                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.a & 0x0F) + (byte & 0x0F) + old_cf) > 0x0F, HALF_CARRY_FLAG);
                set_flag(((registers.a & 0xFF) + (byte & 0xFF) + old_cf) > 0xFF, CARRY_FLAG);

                registers.a = static_cast<uint8_t>(result);
            }
            break;
        case 0xDE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                uint16_t result = registers.a - byte - get_flag(CARRY_FLAG);

                set_flag((result & 0xFF) == 0, ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag(((registers.a & 0x0F) - (byte & 0x0F) - get_flag(CARRY_FLAG)) < 0, HALF_CARRY_FLAG);
                set_flag(result < 0, CARRY_FLAG);

                registers.a = static_cast<uint8_t>(result);
            }
            break;
        case 0xEE:
            {
                uint8_t result = registers.a ^ GET_BYTE(buffer, pc);
                set_flag(result == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);

                registers.a = result;
            }
            break;
        case 0xFE:
            {
                uint8_t byte = GET_BYTE(buffer, pc);
                set_flag((registers.a - byte) == 0, ZERO_FLAG);
                set_flag(1, SUB_FLAG);
                set_flag((registers.a & 0x0F) < (byte & 0x0F), HALF_CARRY_FLAG);
                set_flag(registers.a < byte, CARRY_FLAG);
            }
            break;
        case 0xE8:
            {
                int8_t byte = static_cast<int8_t>(GET_BYTE(buffer, pc));
                uint16_t result = registers.sp + byte;

                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.sp & 0x0F) + (byte & 0x0F)) > 0x0F, HALF_CARRY_FLAG);
                set_flag(((registers.sp & 0xFF) + (byte & 0xFF)) > 0xFF, CARRY_FLAG);

                registers.sp = result;
            }
            break;
        case 0xF8:
            {
                int8_t signed_byte = static_cast<int8_t>(GET_BYTE(buffer, pc));
                set_flag(0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(((registers.sp & 0x0F) + (signed_byte & 0x0F)) > 0x0F, HALF_CARRY_FLAG);
                set_flag(((registers.sp & 0xFF) + (signed_byte & 0xFF)) > 0xFF, CARRY_FLAG);
                registers.hl = registers.sp + signed_byte;
            }
            break;
        case 0xF9:
            {
                registers.sp = registers.hl;
            }
            break;
        case 0xC7:
        case 0xD7:
        case 0xE7:
        case 0xF7:
        case 0xCF:
        case 0xDF:
        case 0xEF:
        case 0xFF:
            {
                CALL(opcode & 0x38);
            }
            break;
        case 0xF3:
            enable_interrupt = false;
            break;
        case 0xFB:
            enable_interrupt = true;
            break;
        case 0x00:
        case 0x10:
            break;
        case 0xCB:
            {
                uint8_t cb_opcode = memory_read(pc + 1);
                process_prefix_instructions(cb_opcode);
                *instr = instr_prefix_lut[cb_opcode];
            }
            break;
        case 0x76:
            {
               is_halted = true; 
            }
            break;
        default:
            std::cout << std::format("Unimplemented Instruction 0x{:02X}\n", opcode);
    }
}

void GameBoy::process_prefix_instructions(uint8_t opcode) {
    InstrInfo_t instr = instr_prefix_lut[opcode];

    switch(opcode) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t b7 = (*src >> 7);
                *src = (*src << 1) | b7;

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b7, CARRY_FLAG);
            }
            break;
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t b0 = *src & 1; 
                *src = (*src >> 1) | (b0 << 7);

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
            }
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t carry = get_flag(CARRY_FLAG);
                uint8_t b7 = (*src >> 7);
                *src = (*src << 1) | carry;

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b7, CARRY_FLAG);
            }
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t old_cf = get_flag(CARRY_FLAG);
                uint8_t b0 = *src & 1; 
                *src = (*src >> 1) | (old_cf << 7);

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
            }
            break;
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t b7 = (*src >> 7);
                *src <<= 1;

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b7, CARRY_FLAG);
            }
            break;
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t b0 = (*src & 0x1);
                *src = (*src >> 1) | (*src & 0x80);

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
            }
            break;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                *src = (*src >> 4) | (*src << 4);


                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(0, CARRY_FLAG);
            }
            break;
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                uint8_t b0 = (*src & 0x1);
                *src = (*src >> 1) & ~(1 << 7);

                set_flag(*src == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(0, HALF_CARRY_FLAG);
                set_flag(b0, CARRY_FLAG);
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
        case 0x76:
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
                uint8_t *src = get_src_reg_u8(opcode);
                set_flag(((*src >> GET_DST(opcode)) & 0x1) == 0, ZERO_FLAG);
                set_flag(0, SUB_FLAG);
                set_flag(1, HALF_CARRY_FLAG);
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
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                *src = (*src) & ~(1 << GET_DST(opcode));
            }
            break;
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
            {
                uint8_t *src = get_src_reg_u8(opcode);
                *src = (*src) | (1 << GET_DST(opcode));
            }
            break;
        default:
            std::cout << std::format("Unimplemented Prefix Instruction 0x{:02X}\n", opcode);
    }
}

void GameBoy::Disassemble() {
    uint16_t pc = registers.pc;
    uint8_t opcode = memory_read(pc);
    InstrInfo_t instr = instr_lut[opcode];
    
    jumping = false;
    process_instructions(pc, &instr);

    add_to_cycle(instr.cycles ? instr.cycles : 1);

#ifdef DEBUG
    std::cout << std::format("AF: 0x{:04X} BC: 0x{:04X} DE: 0x{:04X} HL: 0x{:04X} PC: 0x{:04X} SP: 0x{:04X} {}{}{}{} | {:02X} {:02X} {:02X} {:02X}\n", registers.af.raw, registers.bc.raw, registers.de.raw, registers.hl.raw, pc, registers.sp, ZERO_FLAG(registers.af.f.raw) ? 'Z' : '-', SUB_FLAG(registers.af.f.raw) ? 'N' : '-', HALF_CARRY_FLAG(registers.af.f.raw) ? 'H' : '-', CARRY_FLAG(registers.af.f.raw) ? 'C' : '-', buffer[pc], buffer[pc+1], buffer[pc+2], buffer[pc+3]);
    std::cout << std::format("\t[AF]: 0x{:04X} [BC]: 0x{:04X} [DE]: 0x{:04X} [HL]: 0x{:04X}\n", buffer[registers.af.raw], buffer[registers.bc.raw], buffer[registers.de.raw], buffer[registers.hl.raw]) ;
    std::cout << std::format("\t[a8]: 0x{:02X} [a16]: 0x{:02X}\n", buffer[GET_BYTE(buffer, pc)], buffer[GET_WORD(buffer, pc)]);
#endif

    if(buffer[0xFF02] == 0x81) {
        char c = buffer[0xFF01];
        std::cout << std::format("TEST RESULT: {}\n", c);;
        buffer[0xFF02] = 0;
    }

    if(!jumping) {
        registers.pc += instr.length;
    }
}
