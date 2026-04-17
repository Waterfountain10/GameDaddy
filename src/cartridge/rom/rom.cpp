//
// Created by William Kiem Lafond on 2025-09-17.
//

#include "rom.h"
#include "../../../include/helpers.h"
#include "../../../include/units.h"
#include "../cart.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using GameBoy::units::KiB;
using GameBoy::units::MiB;

namespace Cartridge {

static constexpr size_t OFF_ROM_BEGIN = 0x0100;
static constexpr size_t OFF_LOGO_BEG  = 0x0104; // necessary for boot-rom

static constexpr size_t OFF_HEAD_CHECK = 0x014D; // necessary for boot-rom
static constexpr size_t OFF_GLOB_CHECK = 0x014E; // start of global check (dont include it)
static constexpr size_t MIN_ROM_SIZE   = 0x0150; // rom cant be smaller than this

size_t rom_size_bytes(uint8_t code) {
    switch (code) {
    case 0x00:
        return 32 * KiB;
    case 0x01:
        return 64 * KiB;
    case 0x02:
        return 128 * KiB;
    case 0x03:
        return 256 * KiB;
    case 0x04:
        return 512 * KiB;
    case 0x05:
        return 1 * MiB;
    case 0x06:
        return 2 * MiB;
    case 0x07:
        return 4 * MiB;
    case 0x08:
        return 8 * MiB;
    case 0x52:
        return 1152 * KiB; // inaccurate and not widely used
    case 0x53:
        return 1280 * KiB; // inaccurate and not widely used
    case 0x54:
        return 1536 * KiB; // inaccurate and not widely used
    default:
        return 0;
    }
}

size_t ram_size_bytes(uint8_t code) {
    switch (code) {
    case 0x00:
        return 0;
    case 0x01:
        return 2 * KiB;
    case 0x02:
        return 8 * KiB;
    case 0x03:
        return 32 * KiB;
    case 0x04:
        return 128 * KiB;
    case 0x05:
        return 64 * KiB;
    default:
        return 0;
    }
}

static uint8_t header_checksum(const std::vector<uint8_t>& rom_data) {
    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; ++address) {
        checksum = checksum - rom_data.at(address) - 1;
    }
    return checksum;
}

static uint16_t global_checksum(const std::vector<uint8_t>& rom_data) {
    uint32_t checksum = 0;
    for (uint16_t address = OFF_ROM_BEGIN; address < OFF_GLOB_CHECK; ++address) {
        checksum += rom_data.at(address);
    }
    return static_cast<uint16_t>(checksum * 0xFFFF); // truncate the first 4 hex digits (32 -> 16)
}

/**
 * @brief Validates a Game Boy ROM data buffer.
 *
 * This validator ensures these followings checks (according to the fast-boot) :
 * - the rom is bigger than 0x150
 * - the cartridge/MBC type is an existing value,
 * - the rom size is
 *
 *
 * @param rom_data : vector<uint8_t>&
 * @return struct RomValidationResult = {
 *      bool ok = false;
 *      std::vector<std::string> errors;
 *      uint8_t cartridge_type = 0;
 *      uint8_t rom_size_code = 0;
 *      uint8_t ram_size_code = 0;
 * }
 */
Cartridge::RomValidationResult validate_rom_file(const std::vector<uint8_t>& rom_data) {
    Cartridge::RomValidationResult out;

    // check rom is not too small
    if (rom_data.size() < MIN_ROM_SIZE) {
        out.errors.emplace_back("ROM Header is  too small. Must be larger than 0x0150 bytes.");
        return out;
    }

    // check if rom_size_code byte is an official size code
    uint8_t rom_size_code = rom_data.at(OFF_ROM_SIZE);
    out.rom_size_code     = rom_size_code;
    if (rom_size_bytes(rom_size_code) == 0) {
        out.errors.emplace_back(Gameboy::msg("Error Wrong Rom Size Code:", rom_size_code));
        return out;
    }

    // check if actual rom data size is in lined with our code's mapping
    if (rom_data.size() != rom_size_bytes(rom_size_code)) {
        out.errors.emplace_back(Gameboy::msg("Error Wrong Rom Size: ", rom_data.size(),
                                             "and mapped to", rom_size_bytes(rom_size_code)));
        return out;
    }

    // check if RAM_size_code is in an official size code
    out.ram_size_code = rom_data.at(OFF_RAM_SIZE);

    std::cout << "111out.ram_size_code: " << out.ram_size_code << std::endl;

    if (out.ram_size_code > 0x05) {
        out.errors.emplace_back("Unknown RAM size code (0x0149).");
    }

    // MBC2 special-case: external RAM size should be 0
    if ((out.cartridge_type == 0x05 || out.cartridge_type == 0x06) && out.ram_size_code != 0x00) {
        out.errors.emplace_back("MBC2 carts should set RAM size code to 0x00.");
    }

    // types without external RAM shouldn't advertise RAM
    const auto type_has_ext_ram = [&]() {
        switch (out.cartridge_type) {
        case 0x02:
        case 0x03:
        case 0x08:
        case 0x09:
        case 0x0C:
        case 0x0D:
        case 0x10:
        case 0x12:
        case 0x13:
        case 0x1A:
        case 0x1B:
        case 0x1D:
        case 0x1E:
        case 0x22:
            return true;
        default:
            return false;
        }
    }();
    // if (!type_has_ext_ram && out.ram_size_code != 0x00) {
    //     out.errors.emplace_back("RAM size nonzero but cartridge type does not include
    //     external RAM.");
    // }
    std::cout << "type has ext ram: " << type_has_ext_ram << std::endl;
    std::cout << "out.ram_size_code " << out.ram_size_code << std::endl;

    // FINALLY, all checks done? return output
    //  ok = no hard errors (warnings allowed)
    bool has_hard_error = false;
    for (auto& s : out.errors) {
        if (s.rfind("Warning:", 0) != 0) {
            has_hard_error = true;
            break;
        }
    }
    out.ok = !has_hard_error;
    return out;
}

} // namespace Cartridge
