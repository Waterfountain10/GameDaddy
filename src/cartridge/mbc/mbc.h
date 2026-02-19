//
// Created by William Lafond on 2025-07-08.
//

#pragma once
#include <cstdint>
#include <vector>

namespace Cartridge {

// MBC Interface (pure virtual/abstract) ---------------------------------------------------------------------
class MBC {
protected:
    MBC()                      = default;

public:
    virtual ~MBC()             = default;

    MBC(const MBC&)            = delete;    // mbc cant be copied via:      MBC new_mbc(mbc)
    MBC& operator=(const MBC&) = delete;    // mbc cant be copy-assign via: new_mbc = mbc
    MBC(MBC&&)                 = delete;    // mbc cant be moved via:       MBC new_mbc = std::move(mbc)
    MBC& operator=(MBC&&)      = delete;    // mbc cant be move-assign via: new_mbc = std::move(mbc)

    virtual uint8_t read(uint16_t addr) = 0;
    virtual void    write(uint16_t addr, uint8_t value) = 0;
};


/// POLY IMPLEMENTATIONS OF MBC INTERFACE: ---------------------------------------------------------------------

// ROM ONLY (0x00)
//  - no bank,
//  - no external ram (or optional)
class RomOnly final : public MBC {
public:
    explicit RomOnly(const std::vector<uint8_t>& rom,
                           std::vector<uint8_t>& ram)
        : rom_(rom), ram_(ram) {}

    uint8_t read(uint16_t addr) override;
    void    write(uint16_t addr, uint8_t value) override;

private:
    const std::vector<uint8_t>& rom_;
          std::vector<uint8_t>& ram_;
};


// MBC1 (0x01)
class MBC1 final : public MBC {
public:
    explicit MBC1(const std::vector<uint8_t>& rom,
                        std::vector<uint8_t>& ram)
        : rom_(ram), ram_(ram) {}

    uint8_t read(uint16_t addr) override;
    void    write(uint16_t addr, uint8_t value) override;

private:
    const std::vector<uint8_t>& rom_;
          std::vector<uint8_t>& ram_;

    bool ram_enabled_ = false;
    // MBC1 registers
    uint8_t rom_bank_low5_ = 1; // 5 bits
    uint8_t bank_high2_ = 0;    // 2 bits
    uint8_t mode_ = 0;          // 0=ROM
    uint32_t rom_bank_count_ = 0;
    uint32_t ram_bank_count_ = 0;
    uint32_t clamp_rom_bank_(uint32_t bank) const;
    uint32_t clamp_ram_bank_(uint32_t bank) const;
};
}
