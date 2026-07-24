#pragma once

#include <memory>
#include "common.hpp"
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

#define ZERO_FLAG(flags)        (((flags) >> 7) & 1)
#define SUB_FLAG(flags)         (((flags) >> 6) & 1) // used for BCD
#define HALF_CARRY_FLAG(flags)  (((flags) >> 5) & 1) // used for BCD
#define CARRY_FLAG(flags)       (((flags) >> 4) & 1)

typedef union {
    struct {
        uint8_t unused:4;
        uint8_t c:1;
        uint8_t h:1;
        uint8_t n:1;
        uint8_t z:1;
    };
    uint8_t raw;
} Flags_t;

typedef struct registers_t {
    union {
        uint16_t raw = 0;
        struct {
            Flags_t f;
            uint8_t a;
        };
    } af;

    union {
        uint16_t raw = 0;
        struct {
            uint8_t c;
            uint8_t b;
        };
    } bc;

    union {
        uint16_t raw = 0;
        struct {
            uint8_t e;
            uint8_t d;
        };
    } de;

    union {
        uint16_t raw = 0;
        struct {
            uint8_t l;
            uint8_t h;
        };
    } hl;

    uint16_t sp = 0;
    uint16_t pc = 0x0100;
} Registers_t;

class GameBoy {
public:
    GameBoy(std::string file_name);
    ~GameBoy();
    void Run();

    uint8_t Disassemble(uint16_t pc);

    const Flags_t get_flags() { return registers.af.f; }
    const uint64_t get_cycles() { return tot_cycles; }
private:
    void set_pc(uint16_t pc) { registers.pc = pc; }
    void add_to_cycle(uint8_t inc) { tot_cycles += inc; }
    Registers_t registers;
    uint64_t tot_cycles = 0;

    std::unique_ptr<uint8_t[]> buffer;

    bool enable_interrupt = false;

    std::ifstream gb_file;
    uint64_t      gb_file_size;
    std::string   file_name;
};

#define POP_SP(data, buffer, sp)                                \
    data = (buffer[sp+1] << 8) | buffer[sp];                    \
    sp += 2;                                                    \

#define PUSH_SP(data, buffer, sp)                                \
    sp -= 2;                                                     \
    buffer[sp] = data & 0xFF;                                    \
    buffer[sp+1] = (data >> 8) & 0xFF;                           \

