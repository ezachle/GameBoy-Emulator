#pragma once

#include "common.hpp"
#include "mmu.hpp"
#include "registers.hpp"
#include "opcode.hpp"

/*
 * Game Boy specs
 *  Master Clock - 4.194304 MHz
 *  System Clock - 1/4 of Master Clock
 *  WRAM - 8 KiB
 *  VRAM - 8 KiB
 *
 *  Resolution - 160x144
 *
 *  HSync - 9.198 KHz
 *  VSync - 59.75 Hz
 *
 *  Sound - 4 Channels Stereo
 *
 */

class GameBoy {
public:
    GameBoy(std::string file_name);
    ~GameBoy();
    void Run();

    void Disassemble();

    void set_flag(bool condition, uint8_t bit) { 
        if (condition) registers.f |= bit;
        else registers.f &= ~bit;
        registers.f &= 0xF0;
    }
    const uint8_t get_flag(uint8_t bit) { return (registers.f & bit) != 0; }
    const uint64_t get_cycles() { return tot_cycles; }
private:
    MMU mmu;
    void set_pc(uint16_t pc) { registers.pc = pc; }
    void add_to_cycle(uint8_t inc) { tot_cycles += inc; }

    void process_instructions(uint16_t pc, InstrInfo_t *instr);
    void process_prefix_instructions(uint8_t opcode);

    uint16_t pop();
    void push(uint16_t data);
    void call(uint16_t addr);

    uint8_t get_src_reg_u8(uint8_t opcode);
    uint8_t* get_dst_reg_u8(uint8_t opcode);

    Registers_t registers;
    uint64_t tot_cycles = 0;

    bool jumping = false;
    bool enable_interrupt = false;
    bool is_halted = false;
};

