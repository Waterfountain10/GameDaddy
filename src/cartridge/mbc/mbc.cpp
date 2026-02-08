//
// Created by William Lafond on 2025-07-08.
//

#include "mbc.h"

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace Cartridge {

static uint32_t rom_bank_count_from_bytes(std::size_t rom_size) {
    // each bank is 16KB
    return static_cast<uint32_t>(rom_size / 0x4000);
}

static uint32_t ram_bank_count_bytes(std::size_t ram_size) {
    // each RAM bank is 8KB. (except for MBC2, there are 2KB)
    if (ram_size == 0) return 0;
    if (ram_size <= 0x2000) return 1;
    return static_cast<uint32_t>(ram_size / 0x2000);
}

MBC1::MBC1(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram)
    : rom_(rom), ram_(ram)
{
    // TODO constructor init functions

}

uint32_t MBC1::clamp_rom_bank_(uint32_t bank) const {
    // TODO
    return 0;
}

uint32_t MBC1::clamp_ram_bank_(uint32_t bank) const {
    // TODO
    return 0;
}

void MBC1::write(uint16_t addr, uint8_t value) {
    // TODO
    return;
}

uint8_t MBC1::read(uint16_t addr) {
    // TODO
    return 0;
}


}
