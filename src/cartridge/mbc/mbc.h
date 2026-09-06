//
// Created by William Lafond on 2025-07-08.
//

#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace Cartridge {

/// @brief Abstract Interface for Game Boy cartridge's Memory Bank Controller (MBC)
///
/// A MBC is responsible for translating CPU-visible addresses into read/write operations
/// which
///     - 0x0000-0x3fff : fixed ROM bank region
///     - 0x4000-0x7fff : switchable ROM
///     - 0xA000-0xbfff : external cartridge RAM region
///
///
/// For cartridges without a memory bank controller ("ROM ONLY"), accesses in
/// 0x0000–0x7FFF map directly to ROM bytes and writes in that region are ignored.
///
/// For cartridges with an MBC (such as MBC1), writes to specific subranges of
/// 0x0000–0x7FFF do not write into ROM. Instead, those writes update internal
/// controller registers, which in turn change which ROM or RAM bank is visible
/// to the CPU.
///
/// This interface allows the rest of the emulator (CPU/MMU/Bus) to treat all
/// cartridge types uniformly: it only performs reads and writes through this
/// abstraction and does not need to know which concrete cartridge hardware is
/// present.
class MBC {

    // The reason we have protected comes from a key cpp feature:
    // The language forbids virtual contstructors (for destructors, its okay
    // since compiler automatically generates one from the constructor if needed)
    // Also, we only want it's subclasses to access MBC() mostly to override it.
protected:
    MBC() = default;

public:
    virtual ~MBC() = default; // destructor must be impl for each subclasses

    MBC(const MBC&)            = delete; // mbc cant be copied via:      MBC new_mbc(mbc)
    MBC& operator=(const MBC&) = delete; // mbc cant be copy-assign via: new_mbc = mbc
    MBC(MBC&&)            = delete; // mbc cant be moved via:       MBC new_mbc = std::move(mbc)
    MBC& operator=(MBC&&) = delete; // mbc cant be move-assign via: new_mbc = std::move(mbc)

    virtual uint8_t read(uint16_t addr)                 = 0;
    virtual void    write(uint16_t addr, uint8_t value) = 0;
};

// ---------------------------------------------------------------------
// POLY IMPLEMENTATIONS OF MBC INTERFACE:
// ---------------------------------------------------------------------

/// @brief ROM ONLY (0x00)
///
/// This is the simplest cart config. It contains no bank-switching MBC,
/// the ROM is directly exposed to the CPU. For games up to 32 KiB.
///
///     - 0x0000-0x7fff : direct rom mapping
///     - 0xa000-0xbfff : usually absent (read returns trivial 0xff, write does nothing)
///
class RomOnly final : public MBC {
public:
    explicit RomOnly(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram)
        : rom_(rom), ram_(ram) {}

    uint8_t read(uint16_t addr) override;
    void    write(uint16_t addr, uint8_t value) override;

private:
    const std::vector<uint8_t>& rom_;
    std::vector<uint8_t>&       ram_;
};

/// @brief MBC1 (0x01)
///
/// MBC1 allows Game Boy cartridges larger than 32 KiB to expose multiple ROM
/// banks to the CPU. Because the Game Boy CPU can only address 32 KiB of ROM
/// space at once,
/// the MBC swaps different banks of ROM (cpu) <-> "switchable window" (cart).
///
/// Address layout seen by the CPU:
///   0x0000–0x3FFF : #1 Fixed ROM bank (normally bank 0)
///   0x4000–0x7FFF : #2 Switchable ROM bank window
///   0xA000–0xBFFF : #3 External RAM window (if the cartridge has RAM)
///
/// ---------------------------------------------------------------------------
/// How bank switching works
///
/// The CPU does not directly select a bank number (like 1,2, or 3). Instead,
/// it writes to control registers inside cart. These registers change the internal
/// state of the MBC. (sort of like flags)
///
/// Control register writes:
///
///   0x0000–0x1FFF : RAM enable register
///                   Enables or disables external cartridge RAM (bank #3)
///
///   0x2000–0x3FFF : ROM bank register (but only lower 5 bits)
///                   Selects the main ROM bank number for the switchable window.
///
///   0x4000–0x5FFF : Upper bank bits
///                   Used either as:
///                     - additional ROM bank bits
///                     - RAM bank selection
///                   depending on the current banking mode.
///
///   0x6000–0x7FFF : Banking mode register (this is main flag!)
///                   Selects how the upper bank bits (#2) are interpreted.
///
/// ---------------------------------------------------------------------------
/// Banking modes
///
/// MBC1 supports two modes which determine how the upper bank bits are used.
///
/// Mode 0 — ROM banking mode
///   - Maximizes the number of ROM banks.
///   - Upper bank bits extend the ROM bank number.
///   - External RAM uses a single bank.
///
/// Mode 1 — RAM banking mode
///   - Allows switching between multiple RAM banks.
///   - Upper bank bits select the RAM bank instead of extending the ROM bank.
///   - Fewer ROM banks are available.
///
/// ---------------------------------------------------------------------------
/// Important MBC1 hardware quirk
///
/// ROM bank 0 cannot appear in the switchable window (0x4000–0x7FFF).
/// If the game requests bank 0, the controller automatically remaps it to bank 1.
///
/// ---------------------------------------------------------------------------
/// Implementation note
///
/// This class models the MBC1 as a small state machine. "CPU writes" update
/// internal registers (RAM enable, ROM bank bits, RAM bank bits, mode),
/// and reads use that state to determine which physical ROM or RAM bank
/// should be accessed.
class MBC1 final : public MBC {
public:
    explicit MBC1(const std::vector<uint8_t>& rom, std::vector<uint8_t>& ram);

    uint8_t read(uint16_t addr) override;
    void    write(uint16_t addr, uint8_t value) override;

private:
    const std::vector<uint8_t>& rom_;
    std::vector<uint8_t>&       ram_;

    // MBC1 registers
    bool    ram_enabled_   = false;
    uint8_t rom_bank_low5_ = 1; // 5 bits
    uint8_t bank_high2_    = 0; // 2 bits
    uint8_t mode_          = 0; // 0=ROM

    uint32_t rom_bank_count_ = 0;
    uint32_t ram_bank_count_ = 0;
    uint32_t clamp_rom_bank_(uint32_t bank) const;
    uint32_t clamp_ram_bank_(uint32_t bank) const;
};

/// @brief MBC2 (0x05, 0x06)
///
/// MBC2 has simple ROM banking plus tiny built-in RAM. It does not use the
/// cartridge RAM size byte. The RAM stores 512 4-bit values.
class MBC2 final : public MBC {
public:
    explicit MBC2(const std::vector<uint8_t>& rom);

    uint8_t read(uint16_t addr) override;
    void    write(uint16_t addr, uint8_t value) override;

private:
    const std::vector<uint8_t>& rom_;
    std::array<uint8_t, 512>    ram_{};

    bool     ram_enabled_    = false;
    uint8_t  rom_bank_       = 1; // lower 4 bits, bank 0 remaps to 1
    uint32_t rom_bank_count_ = 0;

    uint32_t clamp_rom_bank_(uint32_t bank) const;
};

} // namespace Cartridge
