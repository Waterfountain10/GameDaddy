/// We removed the SGB functionality

#pragma once
#include "mbc/mbc.h"
#include <cstdint>
#include <vector>

namespace Cartridge {

class Cart {
  public:
    explicit Cart(std::vector<uint8_t> rom);

    // forward functions to mbc_
    uint8_t call_read(uint16_t addr);
    void call_write(uint16_t addr, uint8_t value);

    // state-safe getters
    uint8_t cart_type() const {
      return cart_type_;
    }
    uint8_t rom_size_code() const {
      return rom_size_code_;
    }
    uint8_t ram_size_code() const {
      return ram_size_code_;
    }

  private:
    uint8_t cart_type_ = 0;     // byte at 0x0147 ex: 0x00 for ROM ONLY
    uint8_t rom_size_code_ = 0; // ''      0x0148
    uint8_t ram_size_code_ = 0; // ''      0x0149

    std::vector<uint8_t> rom_;
    std::vector<uint8_t> ram_;
    std::unique_ptr<MBC> mbc_;

    void attach_mbc_();
    void alloc_ram_();
};

} // namespace Cartridge
