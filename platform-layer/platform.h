//
// Created by William Lafond on 2025-07-08.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#include <memory>
#include "display/display_interface.h"

namespace GameBoy {
class CPU;

class Platform {
public:
    explicit Platform(std::shared_ptr<CPU> cpu_instance); // TODO : add the other hardware parts
    void setDisplay(std::shared_ptr<DisplayInterface> display);
    void run_frame();
    void run(); // TODO: remove after implementing real game loop
    std::shared_ptr<DisplayInterface> display_; // TODO : move this into private and add public methods using it
private:
    const std::shared_ptr<GameBoy::CPU> cpu_;
    //const std::shared_ptr<GameBoy::PPU> ppu_;
    //const std::shared_ptr<GameBoy::JoyPad> joypad_;
    //const std::shared_ptr<GameBoy::Sound> sound_; // sound chip hardware
    // Dummy values for test rendering
    int center_col = 80;  // Middle of 160px width
    int center_row = 72;  // Middle of 144px height
    int scale = 1;
    std::chrono::microseconds cycle_period = std::chrono::microseconds(16'666); // ~60fps
};
}

#endif //PLATFORM_H