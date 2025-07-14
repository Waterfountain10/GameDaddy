//
// Created by William Lafond on 2025-07-08.
//

#include "platform.h"

#include <iostream>

#include "gui.h"

namespace GameBoy {
    Platform::Platform(
        std::shared_ptr<GameBoy::CPU> cpu_instance,
        std::shared_ptr<GameBoy::PPU> ppu_instance,
        std::shared_ptr<GameBoy::JoyPad> joypad_instance,
        std::shared_ptr<GameBoy::Sound> sound_instance,
        std::shared_ptr<Gui> gui_instance
        ) : // member initializer list
    cpu_ { cpu_instance },
    ppu_ { ppu_instance },
    joypad_ { joypad_instance },
    sound_ { sound_instance },
    gui_ { gui_instance } {
        // constructor body
        if (!cpu_ || !ppu_ || !joypad_ || !sound_ || !gui_) {
            std::cerr << "Platform: Invalid instantiation of Platform Layer\n" << std::endl;
            return;
        }
    }

}
