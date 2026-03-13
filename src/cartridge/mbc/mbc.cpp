//
// Created by William Lafond on 2025-07-08.
//

#include "mbc.h"

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace Cartridge {

namespace { // helpers for byte calculations

static uint32_t rom_bank_count_from_bytes(std::size_t rom_size) {
    return rom_size / 0x4000; // each ROM bank is 16KB.
}

static uint32_t ram_bank_count_bytes(std::size_t ram_size) {
    if (ram_size == 0)
        return 0;
    if (ram_size <= 0x2000)
        return 1;
    return ram_size / 0x2000; // each RAM bank is 8KB
}
} // namespace

// ---------------------------------------------------------
// ROM ONLY (0x00)
// ---------------------------------------------------------
uint8_t RomOnly::read(uint16_t addr) {
    if (addr <= 0x7FFF) {
        if (addr > rom_.size()) { // edge case: inside the valid direct mapping, but after the last rom byte
            return 0xFF;          // pull high
        }
        return rom_[addr]; // direct mapping else (correct rom-only mapping)
    } else {               // pull high everywhere else (ram location included)
        return 0xFF;
    }
}

void RomOnly::write(uint16_t addr, uint8_t value) {
    return; // no write in rom (a.k.a read-ONLY-memory
}

// ---------------------------------------------------------
// MBC1 - (0x01)
// ---------------------------------------------------------

MBC1::MBC1(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : rom_(rom), ram_(ram) {
    rom_bank_count_ = rom_bank_count_from_bytes(rom_.size());
    ram_bank_count_ = ram_bank_count_bytes(ram_.size());
}

uint32_t MBC1::clamp_rom_bank_(uint32_t bank) const {
    if (rom_bank_count_ == 0)
        return 0;

    bank %= rom_bank_count_;
    if (bank == 0)
        bank = 1;

    return bank;
}

uint32_t MBC1::clamp_ram_bank_(uint32_t bank) const {
    if (ram_bank_count_ == 0)
        return 0;

    bank %= ram_bank_count_;
    return bank;
}

void MBC1::write(uint16_t addr, uint8_t value) {
    // TODO
    return;
}

uint8_t MBC1::read(uint16_t addr) {
    // TODO
    return 0;
}

} // namespace Cartridge
