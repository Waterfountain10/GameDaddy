#include "bus.h"

namespace GameBoy {
// instead of CPU -> ppu/ram/cart...

// CPU → MMU → (PPU, Timer, Cart, RAM)

// 0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
// 4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank
//
// 8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
//
// A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
//
// C000	CFFF	4 KiB Work RAM (WRAM)
// D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1–7
//
// E000	FDFF	Echo RAM (mirror of C000–DDFF)
//
// FE00	FE9F	Object attribute memory (OAM)
//
// !!FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
//
// FF00	FF7F	I/O Registers
//
// FF80	FFFE	High RAM (HRAM)
//
// FFFF	FFFF    IE
//

Bus::Bus(Cartridge::Cart& cart) : cart_{cart} {};

Bus::Bus(Cartridge::Cart& cart, PPU& ppu) : cart_{cart}, ppu_{&ppu} {};

uint8_t Bus::read8(uint16_t addr) {
    if (addr <= 0x7FFF)
        return cart_.read(addr);
    else if (addr <= 0x9FFF)
        return ppu_ ? ppu_->read(addr) : 0xFF;
    else if (addr <= 0xBFFF)
        return cart_.read(addr);
    else if (addr <= 0xDFFF)
        return wram_[addr - 0xC000];
    else if (addr <= 0xFDFF)
        return wram_[addr - 0xE000];
    else if (addr <= 0xFE9F)
        return ppu_ ? ppu_->read(addr) : 0xFF;
    else if (addr <= 0xFEFF)
        return 0xFF;
    else if (addr >= 0xFF40 && addr <= 0xFF4B)
        return ppu_ ? ppu_->read(addr) : 0xFF;
    else if (addr <= 0xFF7F)
        // TODO: return timer/joypad/interrupts/io read(addr);
        return 0xFF;
    else if (addr <= 0xFFFE)
        return hram_[addr - 0xFF80];
    else if (addr == 0xFFFF)
        // TODO: return interrupts_.read(addr);
        return 0xFF;
    else
        return 0xFF; // default handling
}

void Bus::write8(uint16_t addr, uint8_t value) {
    if (addr <= 0x7FFF)
        cart_.write(addr, value);
    else if (addr <= 0x9FFF && ppu_)
        ppu_->write(addr, value);
    else if (addr <= 0xBFFF)
        cart_.write(addr, value);
    else if (addr <= 0xDFFF)
        wram_[addr - 0xC000] = value;
    else if (addr <= 0xFDFF)
        wram_[addr - 0xE000] = value;
    else if (addr <= 0xFE9F && ppu_)
        ppu_->write(addr, value);
    else if (addr <= 0xFEFF)
        return;
    else if (addr >= 0xFF40 && addr <= 0xFF4B && ppu_)
        ppu_->write(addr, value);
    else if (addr <= 0xFF7F)
        // TODO: timer/joypad/interrupts/io write(addr, value);
        return;
    else if (addr <= 0xFFFE)
        hram_[addr - 0xFF80] = value;
    else if (addr == 0xFFFF)
        // TODO: interrupts_.write(addr, value);
        return;
}
} // namespace GameBoy
