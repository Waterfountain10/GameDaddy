//
// Created by William Kiem Lafond on 2025-09-17.
//

#include "rom-validation.h"
#include "../../include/units.h"
#include <unordered_map>
using GameBoy::units::KiB;
using GameBoy::units::MiB;

namespace Gameboy {

static constexpr size_t OFF_ROM_BEGIN   = 0x0100;
static constexpr size_t OFF_LOGO_BEG    = 0x0104; // necessary for boot-rom
static constexpr size_t OFF_LOGO_END    = 0x0133; // necessary for boot-rom
static constexpr size_t OFF_CARTRIDGE_T = 0x0147;
static constexpr size_t OFF_ROM_SIZE    = 0x0148;
static constexpr size_t OFF_RAM_SIZE    = 0x0149;
static constexpr size_t OFF_HEAD_CHECK  = 0x014D; // necessary for boot-rom
static constexpr size_t OFF_GLOB_CHECK  = 0x014E; // start of global check (dont include it)

static constexpr std::array<uint8_t, 48> NINTENDO_LOGO = {
    0xCE,0xED,0x66,0x66,0xCC,0x0D,0x00,0x0B,0x03,0x73,0x00,0x83,0x00,0x0C,0x00,0x0D,
    0x00,0x08,0x11,0x1F,0x88,0x89,0x00,0x0E,0xDC,0xCC,0x6E,0xE6,0xDD,0xDD,0xD9,0x99,
    0xBB,0xBB,0x67,0x63,0x6E,0x0E,0xEC,0xCC,0xDD,0xDC,0x99,0x9F,0xBB,0xB9,0x33,0x3E
};

static constexpr std::unordered_map<uint8_t, std::string> CARTRIDGE_TYPES = {
    {0x00, "ROM ONLY"},
    {0x01, "MBC1"},
    {0x02, "MBC1+RAM"},
    {0x03, "MBC1+RAM+BATTERY"},
    {0x05, "MBC2"},
    {0x06, "MBC2+BATTERY"},
    {0x08, "ROM+RAM"},
    {0x09, "ROM+RAM+BATTERY"},
    {0x0B, "MMM01"},
    {0x0C, "MMM01+RAM"},
    {0x0D, "MMM01+RAM+BATTERY"},
    {0x0F, "MBC3+TIMER+BATTERY"},
    {0x10, "MBC3+TIMER+RAM+BATTERY"},
    {0x11, "MBC3"},
    {0x12, "MBC3+RAM"},
    {0x13, "MBC3+RAM+BATTERY"},
    {0x19, "MBC5"},
    {0x1A, "MBC5+RAM"},
    {0x1B, "MBC5+RAM+BATTERY"},
    {0x1C, "MBC5+RUMBLE"},
    {0x1D, "MBC5+RUMBLE+RAM"},
    {0x1E, "MBC5+RUMBLE+RAM+BATTERY"},
    {0x20, "MBC6"},
    {0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
    {0xFC, "POCKET CAMERA"},
    {0xFD, "BANDAI TAMA5"},
    {0xFE, "HuC3"},
    {0xFF, "HuC1+RAM+BATTERY"}
};

static constexpr std::unordered_map<uint8_t, size_t> ROM_SIZE = {
    {0x00, 32 * KiB},
    {0x01, 64 * KiB},
    {0x02, 128 * KiB},
    {0x03, 256 * KiB},
    {0x04, 512 * KiB},
    {0x05, 1 * MiB},
    {0x06, 2 * MiB},
    {0x07, 4 * MiB},
    {0x08, 8 * MiB},
    {0x52, 1.1 * MiB}, // inaccurate and not widely used
    {0x53, 1.2 * MiB}, // inaccurate and not widely used
    {0x54, 1.5 * MiB}, // inaccurate and not widely used
};

static constexpr std::unordered_map<uint8_t, size_t> RAM_SIZE = {
    {0x00, 0},
    {0x01, 2 * KiB}, // this has never been used so Pandocs says 'UNUSED'
    {0x02, 8 * KiB},
    {0x03, 32 * KiB},
    {0x04, 128 * KiB},
    {0x05, 64 * KiB}
};

static uint8_t header_checksum(const std::vector<uint8_t>& rom_data) {
    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; ++address) {
        checksum -= rom_data.at(address);
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

// add validation rom
}