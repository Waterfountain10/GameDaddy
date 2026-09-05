//
// Created by William Lafond on 2025-07-08.
//

#include "ppu.h"

namespace GameBoy {

PPU::PPU() {
    registers_[0x00] = 0x91; // LCDC
    registers_[0x01] = 0x85; // STAT upper/default bits
    registers_[0x04] = 0x00; // LY starts at scanline 0
    registers_[0x07] = 0xFC; // BGP default DMG palette
    registers_[0x08] = 0xFF; // OBP0
    registers_[0x09] = 0xFF; // OBP1
}

uint8_t PPU::read(uint16_t addr) const {
    if (addr >= VRAM_START && addr <= VRAM_END)
        return vram_[addr - VRAM_START];
    if (addr >= OAM_START && addr <= OAM_END)
        return oam_[addr - OAM_START];
    if (addr >= REG_START && addr <= REG_END)
        return registers_[addr - REG_START];
    if (addr >= UNUSED_REG_LOW && addr <= UNUSED_REG_HIGH)
        return 0xFF;
    return 0xFF;
}

void PPU::write(uint16_t addr, uint8_t value) {
    if (addr >= VRAM_START && addr <= VRAM_END) {
        vram_[addr - VRAM_START] = value;
        return;
    }
    if (addr >= OAM_START && addr <= OAM_END) {
        oam_[addr - OAM_START] = value;
        return;
    }
    if (addr == LY_REG)
        return;
    if (addr == DMA_REG) {
        registers_[addr - REG_START] = value;
        return;
    }
    if (addr >= REG_START && addr <= REG_END)
        registers_[addr - REG_START] = value;
}

} // namespace GameBoy
