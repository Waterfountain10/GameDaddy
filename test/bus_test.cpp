#include "cartridge/cart.h"
#include "gameboy/bus/bus.h"
#include "gameboy/io/ppu/ppu.h"

#include <gtest/gtest.h>
#include <vector>

namespace {
std::vector<uint8_t> make_rom_only_cart() {
    std::vector<uint8_t> rom(0x8000, 0x00);
    rom[0x0100] = 0xC3;
    rom[0x4000] = 0x42;
    rom[0x0147] = 0x00; // ROM ONLY
    rom[0x0148] = 0x00; // 32 KiB ROM
    rom[0x0149] = 0x00; // no external RAM
    return rom;
}
} // namespace

TEST(BusRouting, RoutesCartridgeRomAndExternalRamWindowsToCart) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::Bus    bus(cart);

    EXPECT_EQ(bus.read8(0x0100), 0xC3);
    EXPECT_EQ(bus.read8(0x4000), 0x42);
    EXPECT_EQ(bus.read8(0xA000), 0xFF); // ROM-only cart has no external RAM
}

TEST(BusRouting, RoutesVramOamAndLcdRegistersToAttachedPpu) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::PPU    ppu;
    GameBoy::Bus    bus(cart, ppu);

    bus.write8(0x8000, 0x12);
    bus.write8(0x9FFF, 0x34);
    bus.write8(0xFE00, 0x56);
    bus.write8(0xFE9F, 0x78);
    bus.write8(0xFF47, 0xE4);

    EXPECT_EQ(bus.read8(0x8000), 0x12);
    EXPECT_EQ(bus.read8(0x9FFF), 0x34);
    EXPECT_EQ(bus.read8(0xFE00), 0x56);
    EXPECT_EQ(bus.read8(0xFE9F), 0x78);
    EXPECT_EQ(bus.read8(0xFF47), 0xE4);
}

TEST(BusRouting, ReturnsHighForPpuRangesWhenNoPpuIsAttached) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::Bus    bus(cart);

    bus.write8(0x8000, 0x12);
    bus.write8(0xFE00, 0x56);
    bus.write8(0xFF47, 0xE4);

    EXPECT_EQ(bus.read8(0x8000), 0xFF);
    EXPECT_EQ(bus.read8(0xFE00), 0xFF);
    EXPECT_EQ(bus.read8(0xFF47), 0xFF);
}

TEST(BusRouting, WramAndEchoRamMirrorEachOther) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::Bus    bus(cart);

    bus.write8(0xC000, 0xAB);
    EXPECT_EQ(bus.read8(0xC000), 0xAB);
    EXPECT_EQ(bus.read8(0xE000), 0xAB);

    bus.write8(0xFDFF, 0xCD);
    EXPECT_EQ(bus.read8(0xDDFF), 0xCD);
    EXPECT_EQ(bus.read8(0xFDFF), 0xCD);
}

TEST(BusRouting, HramReadsBackWrittenBytes) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::Bus    bus(cart);

    bus.write8(0xFF80, 0x99);
    bus.write8(0xFFFE, 0xAA);

    EXPECT_EQ(bus.read8(0xFF80), 0x99);
    EXPECT_EQ(bus.read8(0xFFFE), 0xAA);
}

TEST(BusRouting, UnusableAndUnimplementedIoReadHigh) {
    Cartridge::Cart cart(make_rom_only_cart());
    GameBoy::Bus    bus(cart);

    bus.write8(0xFEA0, 0x11);
    bus.write8(0xFEFF, 0x22);
    bus.write8(0xFF00, 0x33);
    bus.write8(0xFFFF, 0x44);

    EXPECT_EQ(bus.read8(0xFEA0), 0xFF);
    EXPECT_EQ(bus.read8(0xFEFF), 0xFF);
    EXPECT_EQ(bus.read8(0xFF00), 0xFF);
    EXPECT_EQ(bus.read8(0xFFFF), 0xFF);
}
