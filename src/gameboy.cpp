#include "gameboy.hpp"
namespace fs = std::filesystem;

GameBoy::GameBoy(std::string file_name): mmu(), cpu(mmu){
    mmu.load_cartridge(std::make_unique<Cartridge>(file_name));
}

GameBoy::~GameBoy() {
}

void GameBoy::Run() {
    while(running) {
        running = cpu.cpu_step();
    }
}
