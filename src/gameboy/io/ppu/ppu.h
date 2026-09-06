//
// Created by William Lafond on 2025-07-08.
//

#ifndef PPU_H
#define PPU_H

#include <array>
#include <cstdint>

namespace GameBoy {

class PPU {
public:
    PPU();

    uint8_t read(uint16_t addr) const;
    void    write(uint16_t addr, uint8_t value);

private:
    static constexpr uint16_t VRAM_START = 0x8000;
    static constexpr uint16_t VRAM_END   = 0x9FFF;
    static constexpr uint16_t OAM_START  = 0xFE00;
    static constexpr uint16_t OAM_END    = 0xFE9F;
    static constexpr uint16_t REG_START  = 0xFF40;
    static constexpr uint16_t REG_END    = 0xFF4B;
    static constexpr uint16_t LY_REG     = 0xFF44;

    std::array<uint8_t, 0x2000> vram_{};      // 0x8000-0x9FFF
    std::array<uint8_t, 0xA0>   oam_{};       // 0xFE00-0xFE9F
    std::array<uint8_t, 0x0C>   registers_{}; // 0xFF40-0xFF4B
};

} // namespace GameBoy

#endif // PPU_H
