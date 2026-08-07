#include "cartridge.hpp"

Cartridge::Cartridge(std::string file_name) {
    this->file_name = file_name;
    gb_file.open(file_name, std::ios::in | std::ios::binary);
    if(!gb_file.is_open()) {
        throw std::runtime_error(
            "Error: Invalid .gb file\n"
            "Usage: ./GameBoy <path to .GB>"
        );
    }

    gb_file_size = fs::file_size(fs::path(file_name));

    rom_data.resize(0x10000);

    if(!gb_file.read(reinterpret_cast<char*>(rom_data.data()), gb_file_size)) {
        throw std::runtime_error(std::format(
            "Failed to read {} file", file_name
        ));
    }
}

Cartridge::~Cartridge() {}

uint8_t Cartridge::read_memory(uint16_t addr) {
    return rom_data[addr];
}

void Cartridge::write_memory(uint16_t addr, uint8_t data) {
    rom_data[addr] = data;
}
