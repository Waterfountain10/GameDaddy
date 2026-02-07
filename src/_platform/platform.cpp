//
// Created by William Lafond on 2025-07-08.
//

#include "platform.h"

#include <iostream>
#include <SDL_events.h>
#include <SDL_timer.h>
#include <thread>

#include "../gameboy/memory/memory.h"
#include "../cartridge/rom/rom-validation.h"

namespace GameBoy {
// TODO: implement the commented parts
Platform::Platform(
    std::shared_ptr<GameBoy::CPU> cpu_instance,
    std::shared_ptr<GameBoy::Memory> memory_instance
    ):
    cpu_ { cpu_instance },
    memory_ { memory_instance}
//     std::shared_ptr<GameBoy::PPU> ppu_instance,
//     std::shared_ptr<GameBoy::JoyPad> joypad_instance,
//     std::shared_ptr<GameBoy::Sound> sound_instance,
//     ) : // member initializer list
// cpu_ { cpu_instance },
// ppu_ { ppu_instance },
// joypad_ { joypad_instance },
// sound_ { sound_instance },
// gui_ { gui_instance }
{
    // constructor body
    if (!cpu_ | !memory_) {
        std::cerr << "Platform: Invalid instantiation of Platform Layer\n" << std::endl;
        return;
    }
}

void Platform::setDisplay(std::shared_ptr<DisplayInterface> display_instance) {
    display_ = display_instance;
    if (!display_) {
        std::cerr << "Platform: DisplayInterface instance is null\n";
    }
}

void Platform::run_frame() {
    if (!display_) {
        std::cerr << "Platform: Cannot run frame without a display\n";
        return;
    }

    auto frame_start_time = std::chrono::steady_clock::now();

    display_->clear();

    // draw vertical line
    for (int r = center_row - 5; r <= center_row + 5; ++r) {
        display_->draw_pixel(center_col, r, true, 3);  // black
    }

    // draw horizontal line
    for (int c = center_col - 5; c <= center_col + 5; ++c) {
        display_->draw_pixel(c, center_row, true, 3);  // black
    }

    display_->present_idle();

    auto frame_end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frame_end_time - frame_start_time);
    auto time_to_wait = cycle_period - elapsed;

    if (time_to_wait.count() > 0)
        std::this_thread::sleep_for(time_to_wait);
}

void Platform::run() {
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        run_frame();
        SDL_Delay(16);
    }
}

// TODO: change this into attach_cartridge_to_
void Platform::load_rom_into_memory(const std::vector<uint8_t>& rom_data) {
    memory_->load_rom(rom_data);
}

bool Platform::validate_rom_bytes(const std::vector<uint8_t>& rom_data) {
    auto res = GameBoy::validate_rom_file(rom_data);
    // INVALID ROM
    if (res.ok == false) {
        for (std::string& e : res.errors) std::cerr << " - " << e << std::endl;
    }
    return res.ok;
}
} // namespace GameBoy
