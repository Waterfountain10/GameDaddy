#include "cartridge/cart.h"
#include "gameboy/bus/bus.h"
#include "gameboy/io/ppu/ppu.h"

#include <gtest/gtest.h>
#include <vector>

namespace {
std::vector<uint8_t> make_rom_only_cart() {
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x0147] = 0x00; // ROM ONLY
    rom[0x0148] = 0x00; // 32 KiB ROM
    rom[0x0149] = 0x00; // no external RAM
    return rom;
}
} // namespace

TEST(PPURegisters, VramReadsBackWrittenBytes) {
    GameBoy::PPU ppu;

    ppu.write(0x8000, 0x12);
    ppu.write(0x9FFF, 0x34);

    EXPECT_EQ(ppu.read(0x8000), 0x12);
    EXPECT_EQ(ppu.read(0x9FFF), 0x34);
}

TEST(PPURegisters, OamReadsBackWrittenBytes) {
    GameBoy::PPU ppu;

    ppu.write(0xFE00, 0x56);
    ppu.write(0xFE9F, 0x78);

    EXPECT_EQ(ppu.read(0xFE00), 0x56);
    EXPECT_EQ(ppu.read(0xFE9F), 0x78);
}

TEST(PPURegisters, LcdRegistersHaveDmgDefaultsAndReadWriteBehavior) {
    GameBoy::PPU ppu;

    EXPECT_EQ(ppu.read(0xFF40), 0x91); // LCDC
    EXPECT_EQ(ppu.read(0xFF44), 0x00); // LY
    EXPECT_EQ(ppu.read(0xFF47), 0xFC); // BGP

    ppu.write(0xFF40, 0x80);
    ppu.write(0xFF47, 0xE4);
    ppu.write(0xFF48, 0xD2);
    ppu.write(0xFF49, 0x24);

    EXPECT_EQ(ppu.read(0xFF40), 0x80);
    EXPECT_EQ(ppu.read(0xFF47), 0xE4);
    EXPECT_EQ(ppu.read(0xFF48), 0xD2);
    EXPECT_EQ(ppu.read(0xFF49), 0x24);
}

TEST(PPURegisters, LyIsReadOnlyForNow) {
    GameBoy::PPU ppu;

    ppu.write(0xFF44, 0x99);

    EXPECT_EQ(ppu.read(0xFF44), 0x00);
}

TEST(PPUBusRouting, RoutesVramOamAndLcdRegistersToPpu) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::PPU    ppu;
    GameBoy::Bus    bus(cart, ppu);

    bus.write8(0x8000, 0xAB);
    bus.write8(0xFE00, 0xCD);
    bus.write8(0xFF47, 0xE4);

    EXPECT_EQ(bus.read8(0x8000), 0xAB);
    EXPECT_EQ(bus.read8(0xFE00), 0xCD);
    EXPECT_EQ(bus.read8(0xFF47), 0xE4);
}
