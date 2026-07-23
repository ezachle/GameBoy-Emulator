#include "common.hpp"
#include "gameboy.hpp"

int main(int argc, char* argv[]) {
    try {
        if(argc != 2) {
            throw std::runtime_error("Usage: ./GameBoy <path to .GB>");
        }

        GameBoy emu(argv[1]);
        emu.Run();
        exit(0);
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
}
