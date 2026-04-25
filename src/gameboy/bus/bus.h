#include "../../cartridge/cart.h"

namespace GameBoy {

class Bus {
public:
    Bus(Cartridge::Cart& cart);

    uint8_t read8(uint16_t addr);
    void    write8(uint16_t addr, uint8_t value);

private:
    Cartridge::Cart& cart_;
    WRAM&            wram_;
    HRAM&            hram_;
    // Timer& timer_;
    // PPU& ppu_;
    // Joypad& joypad_;
    // InterruptController& interrupts_;
};

} // namespace GameBoy
