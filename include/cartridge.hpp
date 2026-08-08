#pragma once
#include <vector>
#include "common.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class Cartridge {
public:
    Cartridge(std::string file_name);
    ~Cartridge();

    uint8_t read_memory(uint16_t addr);
    void write_memory(uint16_t addr, uint8_t data);
private:
    std::vector<uint8_t> rom_data;
    std::ifstream gb_file;
    uint64_t      gb_file_size;
    std::string   file_name;
};
