//
// Created by William Lafond on 2025-07-08.
//

#pragma once
#include <cstdint>
#include <vector>

namespace Cartridge {

// MBC Interface (pure virtual/abstract)
class MBC {
public:
    virtual ~MBC() = default;
    virtual uint8_t read(uint16_t addr) = 0;
    virtual void write(uint16_t addr, uint8_t value) = 0;
};


class MBC1 final : public MBC {
public:
    MBC1(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram);

    uint8_t read(uint16_t addr) override;
    void write(uint16_t addr, uint8_t value) override;
private:
    const std::vector<uint8_t>& rom_;
    std::vector<uint8_t>& ram_;

    bool ram_enabled_ = false;

    // MBC1 registers
    uint8_t rom_bank_low5_ = 1; // 5 bits
    uint8_t bank_high2_ = 0;    // 2 bits
    uint8_t mode_ = 0;          // 0=ROM\

    uint32_t rom_bank_count_ = 0;
    uint32_t ram_bank_count_ = 0;

    uint32_t clamp_rom_bank_(uint32_t bank) const;
    uint32_t clamp_ram_bank_(uint32_t bank) const;
};
}
