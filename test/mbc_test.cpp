#include "cartridge/cart.h"
#include "cartridge/mbc/mbc.h"

#include <gtest/gtest.h>
#include <vector>

namespace {
std::vector<uint8_t> make_banked_rom(uint8_t cart_type, uint8_t rom_size_code,
                                     uint8_t ram_size_code, size_t bank_count) {
    std::vector<uint8_t> rom(bank_count * 0x4000, 0x00);
    for (size_t bank = 0; bank < bank_count; ++bank) {
        rom[bank * 0x4000]     = static_cast<uint8_t>(bank);
        rom[bank * 0x4000 + 1] = static_cast<uint8_t>(bank >> 8);
    }

    rom[0x0147] = cart_type;
    rom[0x0148] = rom_size_code;
    rom[0x0149] = ram_size_code;
    return rom;
}
} // namespace

TEST(MBC2, SwitchesRomBanksWithLowerFourBits) {
    auto rom = make_banked_rom(0x05, 0x01, 0x00, 4);

    Cartridge::MBC2 mbc(rom);

    EXPECT_EQ(mbc.read(0x4000), 0x01);

    mbc.write(0x2100, 0x02);
    EXPECT_EQ(mbc.read(0x4000), 0x02);

    mbc.write(0x2100, 0x00);
    EXPECT_EQ(mbc.read(0x4000), 0x01);
}

TEST(MBC2, StoresBuiltInNibbleRamWhenEnabled) {
    auto rom = make_banked_rom(0x05, 0x00, 0x00, 2);

    Cartridge::MBC2 mbc(rom);

    EXPECT_EQ(mbc.read(0xA000), 0xFF);

    mbc.write(0x0000, 0x0A);
    mbc.write(0xA000, 0xAB);

    EXPECT_EQ(mbc.read(0xA000), 0xFB);

    mbc.write(0x0000, 0x00);
    EXPECT_EQ(mbc.read(0xA000), 0xFF);
}

TEST(Cart, AttachesMBC2CartridgeTypes) {
    auto rom = make_banked_rom(0x06, 0x01, 0x00, 4);

    Cartridge::Cart cart(std::move(rom));

    cart.write(0x2100, 0x03);
    EXPECT_EQ(cart.read(0x4000), 0x03);
}

TEST(MBC3, SwitchesRomBanksWithSevenBitRegister) {
    auto                 rom = make_banked_rom(0x11, 0x02, 0x00, 8);
    std::vector<uint8_t> ram;

    Cartridge::MBC3 mbc(rom, ram);

    EXPECT_EQ(mbc.read(0x4000), 0x01);

    mbc.write(0x2000, 0x04);
    EXPECT_EQ(mbc.read(0x4000), 0x04);

    mbc.write(0x2000, 0x00);
    EXPECT_EQ(mbc.read(0x4000), 0x01);
}

TEST(MBC3, SelectsExternalRamBanksWhenEnabled) {
    auto                 rom = make_banked_rom(0x13, 0x00, 0x03, 2);
    std::vector<uint8_t> ram(4 * 0x2000, 0xFF);

    Cartridge::MBC3 mbc(rom, ram);

    EXPECT_EQ(mbc.read(0xA000), 0xFF);

    mbc.write(0x0000, 0x0A);
    mbc.write(0x4000, 0x00);
    mbc.write(0xA000, 0x12);
    mbc.write(0x4000, 0x02);
    mbc.write(0xA000, 0x34);

    mbc.write(0x4000, 0x00);
    EXPECT_EQ(mbc.read(0xA000), 0x12);
    mbc.write(0x4000, 0x02);
    EXPECT_EQ(mbc.read(0xA000), 0x34);
}

TEST(MBC3, SelectsRtcRegistersAndAcceptsLatchCommand) {
    auto                 rom = make_banked_rom(0x0F, 0x00, 0x00, 2);
    std::vector<uint8_t> ram;

    Cartridge::MBC3 mbc(rom, ram);

    mbc.write(0x0000, 0x0A);
    mbc.write(0x4000, 0x08); // RTC seconds register
    mbc.write(0xA000, 0x25);
    mbc.write(0x6000, 0x00);
    mbc.write(0x6000, 0x01);

    EXPECT_EQ(mbc.read(0xA000), 0x25);
}

TEST(Cart, AttachesMBC3CartridgeTypes) {
    auto rom = make_banked_rom(0x13, 0x01, 0x03, 4);

    Cartridge::Cart cart(std::move(rom));

    cart.write(0x2000, 0x03);
    EXPECT_EQ(cart.read(0x4000), 0x03);
}

TEST(MBC5, SwitchesRomBanksWithNineBitRegister) {
    auto                 rom = make_banked_rom(0x19, 0x08, 0x00, 512);
    std::vector<uint8_t> ram;

    Cartridge::MBC5 mbc(rom, ram);

    mbc.write(0x2000, 0x02);
    EXPECT_EQ(mbc.read(0x4000), 0x02);

    mbc.write(0x2000, 0x01);
    mbc.write(0x3000, 0x01);
    EXPECT_EQ(mbc.read(0x4000), 0x01);
    EXPECT_EQ(mbc.read(0x4001), 0x01);
}

TEST(MBC5, AllowsRomBankZeroInSwitchableWindow) {
    auto                 rom = make_banked_rom(0x19, 0x01, 0x00, 4);
    std::vector<uint8_t> ram;

    Cartridge::MBC5 mbc(rom, ram);

    mbc.write(0x2000, 0x00);
    mbc.write(0x3000, 0x00);

    EXPECT_EQ(mbc.read(0x4000), 0x00);
}

TEST(MBC5, SelectsExternalRamBanksWhenEnabled) {
    auto                 rom = make_banked_rom(0x1B, 0x00, 0x04, 2);
    std::vector<uint8_t> ram(16 * 0x2000, 0xFF);

    Cartridge::MBC5 mbc(rom, ram);

    EXPECT_EQ(mbc.read(0xA000), 0xFF);

    mbc.write(0x0000, 0x0A);
    mbc.write(0x4000, 0x00);
    mbc.write(0xA000, 0x12);
    mbc.write(0x4000, 0x0F);
    mbc.write(0xA000, 0x34);

    mbc.write(0x4000, 0x00);
    EXPECT_EQ(mbc.read(0xA000), 0x12);
    mbc.write(0x4000, 0x0F);
    EXPECT_EQ(mbc.read(0xA000), 0x34);
}

TEST(Cart, AttachesMBC5CartridgeTypes) {
    auto rom = make_banked_rom(0x1B, 0x01, 0x03, 4);

    Cartridge::Cart cart(std::move(rom));

    cart.write(0x2000, 0x03);
    EXPECT_EQ(cart.read(0x4000), 0x03);
}
