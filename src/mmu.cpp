#include "mmu.hpp"

MMU::MMU() {}
MMU::~MMU() {}

void MMU::load_cartridge(std::unique_ptr<Cartridge> cart) {
    this->cart = std::move(cart);
}

uint8_t MMU::get_byte(uint16_t addr) {
    return read_u8(addr + 1);
}

uint16_t MMU::get_word(uint16_t addr) {
    return ((read_u8(addr+2) << 8) | (read_u8(addr+1)));
}

void MMU::write_u8(uint16_t addr, uint8_t data) {
    cart->write_memory(addr, data);

    if(addr == 0xFF02 && (data == 0x81 || data == 0x80)) {
        uint8_t c = cart->read_memory(0xFF01);
        putchar(c);
        fflush(stdout);
        cart->write_memory(0xFF02, 0x01);
    }
}

uint8_t MMU::read_u8(uint16_t addr) {
    // temporary return - 0xFF44 refers to the scanline registers
    // tests expect this to return 0x90
    if(addr == 0xFF44) return 0x90;
    return cart->read_memory(addr);
}

