#pragma once

#include "common.hpp"
#include "mmu.hpp"
#include "cpu.hpp"

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
private:
    bool running = true;
    MMU mmu;
    CPU cpu;
};

