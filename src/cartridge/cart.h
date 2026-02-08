/// We removed the SGB functionality

#pragma once

#include "mbc/mbc.h"
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace Cartridge {

extern const std::unordered_map<uint8_t, std::string> CARTRIDGE_TYPES;

class Cart {
private:
    uint8_t cart_type_; // 0x0147
    uint8_t rom_size_;  // 0x0148
    uint8_t ram_size_;  // 0x0149

    std::vector<uint8_t> rom_;
    std::vector<uint8_t> ram_;

    std::unique_ptr<MBC> mbc_;

public:
    Cart();

    uint8_t call_read(uint8_t);
    uint8_t call_write(uint8_t);
}
