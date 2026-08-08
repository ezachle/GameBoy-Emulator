#pragma once
#include <array>
#include "cartridge.hpp"

// 16-bit address bus
class MMU {
public:
    MMU();
    ~MMU();

    uint8_t get_byte(uint16_t addr);
    uint16_t get_word(uint16_t addr);

    void write_u8(uint16_t addr, uint8_t data);
    uint8_t read_u8(uint16_t addr);

    void load_cartridge(std::unique_ptr<Cartridge> cart);
private:
    std::unique_ptr<Cartridge> cart;

    // used for RAM, ROM and IO
    // https://gbdev.io/pandocs/Memory_Map.html
    std::array<uint8_t, 0x10000> memory;
};
