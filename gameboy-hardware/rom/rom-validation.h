//
// Created by William Kiem Lafond on 2025-09-17.
//

#ifndef ROM_VALIDATION_H
#define ROM_VALIDATION_H
#include <vector>

namespace GameBoy {
struct RomValidationResult {
    bool ok = false;
    std::vector<std::string> errors;
    uint8_t cartridge_type = 0;
    uint8_t rom_size_code = 0;
    uint8_t ram_size_code = 0;
    uint16_t global_checksum = 0;
};
RomValidationResult validate_rom_file(const std::vector<uint8_t>& rom);
};

#endif //ROM_VALIDATION_H
