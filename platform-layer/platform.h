//
// Created by William Lafond on 2025-07-08.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#endif //PLATFORM_H

#include "display/impl/sdl_gui.h"

namespace GameBoy {
class CPU;

class Platform {
private:
    const std::shared_ptr<GameBoy::CPU> cpu_;
    //const std::shared_ptr<GameBoy::PPU> ppu_;
    //const std::shared_ptr<GameBoy::JoyPad> joypad_;
    //const std::shared_ptr<GameBoy::Sound> sound_; // sound chip hardware
public:
};
}