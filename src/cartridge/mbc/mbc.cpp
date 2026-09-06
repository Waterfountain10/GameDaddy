//
// Created by William Lafond on 2025-07-08.
//

#include "mbc.h"

#include <cstddef>
#include <cstdint>

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
        if (addr >= rom_.size()) { // edge case: inside the valid direct mapping, but after the last
                                   // rom byte
            return 0xFF;           // pull high
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
    // ---------------------------------------------------------
    // IMPORTANT:
    // For MBC cartridges, writes in 0x0000–0x7FFF do NOT write into ROM.
    // Instead they update internal controller registers that change
    // which ROM/RAM banks are visible to the CPU.
    //
    // Only the range 0xA000–0xBFFF performs a real memory write
    // (external cartridge RAM).
    // ---------------------------------------------------------

    // Ram-Enable register:
    //      Any value with $A in the lower 4 bits --> enables RAM
    //      ex. 0x2A -> enable ram
    if (addr <= 0x1FFF) {
        ram_enabled_ = ((value & 0x0F) == 0x0A);
    }

    // ROM Bank register:
    //      This 5-bit register selects the rom bank region and discards the 3 highest bits.
    //      ex. 1110001 --> 0001 --> select bank 1
    if (addr <= 0x3FFF) {
        rom_bank_low5_ = value & 0x1F;
        if (rom_bank_low5_ == 0) { // bank 0
            rom_bank_low5_ = 1;    // bank 1
        }
        return;
    }

    // RAM Bank number OR Upper 2 bits of ROM
    //      When RAM: select bank in range of 00-03
    //      When ROM: helps specifies ROM bank number (bit 5 and 6)
    if (addr <= 0x5FFF) {
        bank_high2_ = value & 0x03;
        return;
    }

    // Bank Mode register
    //      This 1-bit Register helps select between default (0), and advanced (1).
    //      For 0: [0000-3FFF] <--ROM / SRAM--> [A000-BFFF]
    //      For 1: [0000-3FFF] <--switchable--> [A000-BFFF]
    if (addr <= 0x7FFF) {
        mode_ = value & 0x01;
        return;
    }
    // ---------------------------------------------------------
    // External RAM region : where the write actually happens.
    // ---------------------------------------------------------
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!ram_enabled_)
            return;

        if (ram_bank_count_ == 0)
            return;

        uint32_t ram_bank = 0;

        if (mode_ == 1)
            ram_bank = bank_high2_;

        ram_bank = clamp_ram_bank_(ram_bank);

        uint32_t offset = ram_bank * 0x2000;
        offset += addr - 0xA000;

        if (offset < ram_.size()) {
            ram_[offset] = value;
        }
    }
}

uint8_t MBC1::read(uint16_t addr) {
    // ---------------------------------------------------------
    // Fixed ROM region: 0x0000 - 0x3FFF
    //
    // In default mode (mode 0), this region always maps to ROM bank 0.
    //
    // In advanced mode (mode 1), the upper bank bits (bank_high2_)
    // extend the bank index, allowing this region to point to
    // higher banks as well.
    //
    // Example:
    //      bank_high2_ = 2
    //      bank = 2 << 5 = 64
    //
    // The final address inside the ROM is:
    //      bank * 16KB + addr
    //
    // clamp_rom_bank_ ensures we never select a bank that does
    // not exist in the cartridge.
    // ---------------------------------------------------------
    if (addr <= 0x3FFF) {
        uint32_t bank = 0;

        if (mode_ == 1)
            bank = bank_high2_ << 5;

        bank = clamp_rom_bank_(bank);

        uint32_t offset = bank * 0x4000;
        offset += addr;

        if (offset < rom_.size())
            return rom_[offset];

        return 0xFF;
    }
    // ---------------------------------------------------------
    // Switchable ROM region: 0x4000 - 0x7FFF
    //
    // This region always maps to a selectable ROM bank.
    //
    // The lower 5 bits come from rom_bank_low5_.
    //
    // In mode 0 (ROM banking mode), the upper two bits are added
    // from bank_high2_ to extend the ROM bank number.
    //
    // Example:
    //      rom_bank_low5_ = 3
    //      bank_high2_ = 2
    //
    //      bank = 3 | (2 << 5) = bank 67
    //
    // clamp_rom_bank_ ensures:
    //      - the bank number stays inside the cartridge range
    //      - bank 0 is never selected (MBC1 hardware rule)
    //
    // The physical ROM address becomes:
    //
    //      bank * 16KB + (addr - 0x4000)
    //
    // because the window itself starts at 0x4000.
    // ---------------------------------------------------------
    if (addr <= 0x7FFF) {

        uint32_t bank = rom_bank_low5_;

        if (mode_ == 0)
            bank |= (bank_high2_ << 5);

        bank = clamp_rom_bank_(bank);

        uint32_t offset = bank * 0x4000;
        offset += addr - 0x4000;

        if (offset < rom_.size())
            return rom_[offset];

        return 0xFF;
    }

    // ---------------------------------------------------------
    // External RAM region: 0xA000 - 0xBFFF
    //
    // This region maps to cartridge RAM if present.
    //
    // RAM must first be enabled via the RAM-enable register.
    //
    // In mode 0:
    //      RAM bank is fixed to bank 0.
    //
    // In mode 1:
    //      bank_high2_ selects the RAM bank (00-03).
    //
    // clamp_ram_bank_ ensures the selected bank does not exceed
    // the amount of RAM physically present in the cartridge.
    //
    // The final RAM offset becomes:
    //
    //      ram_bank * 8KB + (addr - 0xA000)
    //
    // because each RAM bank is 8KB.
    // ---------------------------------------------------------
    if (addr >= 0xA000 && addr <= 0xBFFF) {

        if (!ram_enabled_)
            return 0xFF;

        if (ram_bank_count_ == 0)
            return 0xFF;

        uint32_t ram_bank = 0;

        if (mode_ == 1)
            ram_bank = bank_high2_;

        ram_bank = clamp_ram_bank_(ram_bank);

        uint32_t offset = ram_bank * 0x2000;
        offset += addr - 0xA000;

        if (offset < ram_.size())
            return ram_[offset];

        return 0xFF;
    }

    return 0xFF;
}

// ---------------------------------------------------------
// MBC2 - (0x05, 0x06)
// ---------------------------------------------------------

MBC2::MBC2(const std::vector<uint8_t>& rom) : rom_(rom) {
    rom_bank_count_ = rom_bank_count_from_bytes(rom_.size());
    ram_.fill(0x0F);
}

uint32_t MBC2::clamp_rom_bank_(uint32_t bank) const {
    if (rom_bank_count_ == 0)
        return 0;

    bank %= rom_bank_count_;
    if (bank == 0)
        bank = 1;

    return bank;
}

uint8_t MBC2::read(uint16_t addr) {
    if (addr <= 0x3FFF) {
        if (addr < rom_.size())
            return rom_[addr];
        return 0xFF;
    }

    if (addr <= 0x7FFF) {
        uint32_t bank   = clamp_rom_bank_(rom_bank_);
        uint32_t offset = bank * 0x4000 + (addr - 0x4000);
        if (offset < rom_.size())
            return rom_[offset];
        return 0xFF;
    }

    if (addr >= 0xA000 && addr <= 0xA1FF) {
        if (!ram_enabled_)
            return 0xFF;
        return static_cast<uint8_t>(0xF0 | (ram_[addr - 0xA000] & 0x0F));
    }

    return 0xFF;
}

void MBC2::write(uint16_t addr, uint8_t value) {
    if (addr <= 0x3FFF) {
        if ((addr & 0x0100) == 0) {
            ram_enabled_ = ((value & 0x0F) == 0x0A);
            return;
        }

        rom_bank_ = value & 0x0F;
        if (rom_bank_ == 0)
            rom_bank_ = 1;
        return;
    }

    if (addr >= 0xA000 && addr <= 0xA1FF && ram_enabled_)
        ram_[addr - 0xA000] = value & 0x0F;
}

// ---------------------------------------------------------
// MBC3 - (0x0F - 0x13)
// ---------------------------------------------------------

MBC3::MBC3(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram) : rom_(rom), ram_(ram) {
    rom_bank_count_ = rom_bank_count_from_bytes(rom_.size());
    ram_bank_count_ = ram_bank_count_bytes(ram_.size());
}

uint32_t MBC3::clamp_rom_bank_(uint32_t bank) const {
    if (rom_bank_count_ == 0)
        return 0;

    bank %= rom_bank_count_;
    if (bank == 0)
        bank = 1;

    return bank;
}

uint32_t MBC3::clamp_ram_bank_(uint32_t bank) const {
    if (ram_bank_count_ == 0)
        return 0;

    bank %= ram_bank_count_;
    return bank;
}

uint8_t MBC3::read(uint16_t addr) {
    if (addr <= 0x3FFF) {
        if (addr < rom_.size())
            return rom_[addr];
        return 0xFF;
    }

    if (addr <= 0x7FFF) {
        uint32_t bank   = clamp_rom_bank_(rom_bank_);
        uint32_t offset = bank * 0x4000 + (addr - 0x4000);
        if (offset < rom_.size())
            return rom_[offset];
        return 0xFF;
    }

    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!ram_rtc_enabled_)
            return 0xFF;

        if (ram_rtc_select_ <= 0x03) {
            if (ram_bank_count_ == 0)
                return 0xFF;

            uint32_t ram_bank = clamp_ram_bank_(ram_rtc_select_);
            uint32_t offset   = ram_bank * 0x2000 + (addr - 0xA000);
            if (offset < ram_.size())
                return ram_[offset];
            return 0xFF;
        }

        if (ram_rtc_select_ >= 0x08 && ram_rtc_select_ <= 0x0C)
            return rtc_regs_[ram_rtc_select_ - 0x08];
    }

    return 0xFF;
}

void MBC3::write(uint16_t addr, uint8_t value) {
    if (addr <= 0x1FFF) {
        ram_rtc_enabled_ = ((value & 0x0F) == 0x0A);
        return;
    }

    if (addr <= 0x3FFF) {
        rom_bank_ = value & 0x7F;
        if (rom_bank_ == 0)
            rom_bank_ = 1;
        return;
    }

    if (addr <= 0x5FFF) {
        ram_rtc_select_ = value;
        return;
    }

    if (addr <= 0x7FFF) {
        if (last_latch_ == 0x00 && value == 0x01) {
            // RTC ticking is not implemented yet; latch keeps deterministic skeleton values.
        }
        last_latch_ = value;
        return;
    }

    if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!ram_rtc_enabled_)
            return;

        if (ram_rtc_select_ <= 0x03) {
            if (ram_bank_count_ == 0)
                return;

            uint32_t ram_bank = clamp_ram_bank_(ram_rtc_select_);
            uint32_t offset   = ram_bank * 0x2000 + (addr - 0xA000);
            if (offset < ram_.size())
                ram_[offset] = value;
            return;
        }

        if (ram_rtc_select_ >= 0x08 && ram_rtc_select_ <= 0x0C)
            rtc_regs_[ram_rtc_select_ - 0x08] = value;
    }
}

} // namespace Cartridge
