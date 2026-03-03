//
// Created by William Kiem Lafond on 2025-09-17.
//
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Cartridge {

// Lookup tables for Cartridge
size_t rom_size_bytes(uint8_t code);
size_t ram_size_bytes(uint8_t code);

struct RomValidationResult {
        bool ok = false;
        std::vector<std::string> errors;
        uint8_t cartridge_type = 0;
        uint8_t rom_size_code = 0;
        uint8_t ram_size_code = 0;
};

RomValidationResult validate_rom_file(const std::vector<uint8_t>& rom);
}; // namespace Cartridge
