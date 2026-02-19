/// We removed the SGB functionality

#pragma once
#include "mbc/mbc.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace Cartridge {

inline const std::unordered_map<uint8_t, std::string> CARTRIDGE_TYPES = {
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

class Cart {
public:
    explicit Cart(std::vector<uint8_t> rom);

    // forward functions to mbc_
    uint8_t call_read(uint16_t addr);
    void    call_write(uint16_t addr, uint8_t value);

    // state-safe getters
    uint8_t cart_type()     const { return cart_type_;}
    uint8_t rom_size_code() const { return rom_size_code_;}
    uint8_t ram_size_code() const { return ram_size_code_;}

private:
    uint8_t cart_type_     = 0;  // byte at 0x0147 ex: 0x00 for ROM ONLY
    uint8_t rom_size_code_ = 0;  // ''      0x0148
    uint8_t ram_size_code_ = 0;  // ''      0x0149

    std::vector<uint8_t> rom_;
    std::vector<uint8_t> ram_;
    std::unique_ptr<MBC> mbc_;

    void attach_mbc_();
    void alloc_ram_();
};

}
