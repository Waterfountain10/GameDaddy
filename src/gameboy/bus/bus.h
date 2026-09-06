#include "../../cartridge/cart.h"
#include "../io/ppu/ppu.h"

namespace GameBoy {

class Bus {
public:
    Bus(Cartridge::Cart& cart);
    Bus(Cartridge::Cart& cart, PPU& ppu);

    uint8_t read8(uint16_t addr);
    void    write8(uint16_t addr, uint8_t value);

private:
    Cartridge::Cart& cart_;
    uint8_t          wram_[0x2000];
    uint8_t          hram_[0x7F];
    // Timer& timer_;
    PPU* ppu_ = nullptr;
    // Joypad& joypad_;
    // InterruptController& interrupts_;
};

} // namespace GameBoy
