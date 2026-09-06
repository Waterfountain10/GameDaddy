#ifndef BUS_H
#define BUS_H

#include "../../cartridge/cart.h"
#include "../io/ppu/ppu.h"

#include <array>
#include <cstdint>

namespace GameBoy {

class Bus {
public:
    Bus(Cartridge::Cart& cart);
    Bus(Cartridge::Cart& cart, PPU& ppu);

    uint8_t read8(uint16_t addr);
    void    write8(uint16_t addr, uint8_t value);

private:
    Cartridge::Cart&            cart_;
    std::array<uint8_t, 0x2000> wram_{};
    std::array<uint8_t, 0x7F>   hram_{};
    // Timer& timer_;
    PPU* ppu_ = nullptr;
    // Joypad& joypad_;
    // InterruptController& interrupts_;
};

} // namespace GameBoy

#endif // BUS_H
