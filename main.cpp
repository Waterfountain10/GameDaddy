#include <iostream>
#include <fstream>
#include <format>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>


int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // default values
    std::string rom_path;
    std::ifstream rom_file;

    try {
        switch (argc) {
            case 2: {
                // CLI mode
                rom_path = argv[1];
                rom_file.open(rom_path, std::ios::in | std::ios::binary | std::ios::ate);  // pointing seeker at end

                if (!rom_file.is_open())
                    throw std::runtime_error("<rom_path> file could not be opened.");
                if (rom_file.tellg() > 4000000)
                    throw std::runtime_error("<rom_path> file size is too big for a standard GameBoy ROM (4 MB)");
                if (rom_file.tellg() < 0)
                    throw std::runtime_error("<rom_path> file size is negative");
                break;
            }
            default: {
                throw std::runtime_error("Incorrect number of arguments. Correct usage: ./gamedaddy <ROM_file>>");
            }
        }
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << std::format("Running {}",argv[0]) << std::endl;
        std::cout << std::format("---> ROM: {}", rom_path) << std::endl;
        std::cout << "-------------------------------------------------------" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 0;
    }


    return 0;
}