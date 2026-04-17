#include "cartridge/rom/rom.h"
#include <gtest/gtest.h>

using namespace Cartridge;

// helper for mocking rom
static std::vector<uint8_t> make_rom(uint8_t cart_type, uint8_t rom_size_code,
                                     uint8_t ram_size_code, size_t actual_size) {
    std::vector<uint8_t> rom(actual_size, 0x00);
    if (actual_size >= 0x0150) {
        rom[0x0147] = cart_type;
        rom[0x0148] = rom_size_code;
        rom[0x0149] = ram_size_code;
    }
    return rom;
}

/*
 * Test 1: Reject Too small rom
 */
TEST(RomValidation, RejectsTooSmallRom) {
    std::vector<uint8_t> rom(10, 0x00);
    auto                 result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
    EXPECT_FALSE(result.errors.empty());
}

/*
 * Test 2: Valid Minimal ROM (no MBC/ROM ONLY)
 */
TEST(RomValidation, AcceptsValidRomOnly32KB) {
    auto rom    = make_rom(0x00, // ROM ONLY
                           0x00, // 32KB
                           0x00, // No RAM
                           32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_TRUE(result.ok);
    EXPECT_EQ(result.cartridge_type, 0x00);
    EXPECT_EQ(result.rom_size_code, 0x00);
    EXPECT_EQ(result.ram_size_code, 0x00);
}

/*
 * Test 3: Invalid Cartridge Type
 */
TEST(RomValidation, RejectsUnknownCartridgeType) {
    auto rom    = make_rom(0xAB, // Invalid
                           0x00, 0x00, 32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}

/*
 * Test 4: Invalid Rom size code
 */
TEST(RomValidation, RejectsUnknownRomSizeCode) {
    auto rom    = make_rom(0x00,
                           0xAB, // Invalid size code
                           0x00, 32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}

/*
 * Test 5: mismatched rom atual size(64KB) with claimed (32KB)
 */
TEST(RomValidation, RejectsMismatchedRomSize) {
    auto rom = make_rom(0x00,
                        0x00, // Claims 32KB (if rom_size_code 00, then 32 KiB according to Pandocs)
                        0x00,
                        64 * 1024 // Actually 64KB
    );
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}

/*
 * Test 6: Invalid ram size code (AB does not exist)
 */
TEST(RomValidation, RejectsUnknownRamSizeCode) {
    auto rom    = make_rom(0x00, 0x00, 0xAB, 32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}

/*
 * Test 7: MBC2 must have RAM size == 0
 */
TEST(RomValidation, MBC2MustHaveZeroRamSize) {
    auto rom    = make_rom(0x05, // MBC2
                           0x00,
                           0x02, // Invalid nonzero RAM
                           32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}

/*
 * Test 8: type without RAM, but says at code that it does (ex. ROM ONLY)
 */
TEST(RomValidation, RejectsRamWhenCartDoesNotSupportIt) {
    auto rom    = make_rom(0x00, // ROM ONLY
                           0x00,
                           0x02, // Advertises RAM (not supposed to)
                           32 * 1024);
    auto result = validate_rom_file(rom);

    EXPECT_FALSE(result.ok);
}
