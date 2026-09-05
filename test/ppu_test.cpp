#include "gameboy/io/ppu/ppu.h"

#include <gtest/gtest.h>

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

TEST(PPURegisters, UnusableRangesReadHigh) {
    GameBoy::PPU ppu;

    ppu.write(0xFEA0, 0x11);
    ppu.write(0xFF4C, 0x22);

    EXPECT_EQ(ppu.read(0xFEA0), 0xFF);
    EXPECT_EQ(ppu.read(0xFF4C), 0xFF);
}
