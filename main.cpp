#include <iostream>
#include <fstream>
#include <format>
#include <SDL.h>
#include <SDL_timer.h>

#include "gameboy-hardware/cpu/cpu.h"
#include "gameboy-hardware/memory/memory.h"
#include "platform-layer/platform.h"
#include "platform-layer/display/display_interface.h"
#include "platform-layer/display/impl/sdl_gui.h"

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

    // 1) initialized hardware
    std::shared_ptr<GameBoy::CPU> cpu_instance = std::make_shared<GameBoy::CPU>();
    std::shared_ptr<GameBoy::Memory> memory_instance = std::make_shared<GameBoy::Memory>();
    // TODO : add the rest of the hardware parts

    // 2) initialize gui
    bool use_gui = true; // TODO : Implement a toggle off for CLI mode
    std::shared_ptr<GameBoy::DisplayInterface> screen;
    if (use_gui) {
        screen = std::make_shared<GameBoy::SDLGui>(160, 144);
    } else {
        // TODO Implement the CLI mode
    }

    // 3) initialized the platform
    auto gb_platform = std::make_shared<GameBoy::Platform>(
        cpu_instance,
        memory_instance
    );
    gb_platform->setDisplay(screen);


    // POWER-ON GAMEDADDYYY! ٩(ˊᗜˋ*)ﾉ ---------------------------------------------------------------------------------

    // 1) read rom from path
    std::streamsize rom_size = rom_file.tellg(); // tellg gets pointer position (end of file)
    rom_file.seekg(0, std::ios::beg); // move to beginnging to start reading rom data
    std::vector<uint8_t> rom_data(rom_size);
    rom_file.read(reinterpret_cast<char*>(rom_data.data()), rom_size);

    // 2) validate the rom
    if (!gb_platform->validate_rom_bytes(rom_data))
        throw std::runtime_error("End the program due to failed ROM validation.");

    // 3) load the rom and ram (only after validation)
    gb_platform->load_rom_into_memory(rom_data);
    cpu_instance->attach_memory(memory_instance);

    // 4) load the boot rom (fast boot in this case)
    std::vector<uint8_t> bootrom; // Todo: change temporary to have a CLI parsed args
    cpu_instance->reset_registers_fast(); // now PC=0x0100 (skip boot)

    // 5)  start the game loop
    gb_platform->run(); // TODO: change the actual game loop to run indefinetely (not a fixed timer)
    SDL_Quit();

    // End of all SDL subsystems + destruct layer
    return 0;
}
