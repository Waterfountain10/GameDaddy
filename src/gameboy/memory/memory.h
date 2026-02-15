//
// Created by William Lafond on 2025-07-22.
//

#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cstdint>
#include <vector>

namespace GameBoy {

class Memory {
public:
    // CORE
    Memory();
    static constexpr size_t MEM_SIZE = 0x10000; // 65535 bytes + 1 byte (or 64 KB)
    static constexpr size_t BOOT_ROM_SIZE = 0x100;

    uint8_t read_byte_at(uint16_t address);
    void write_byte_at(uint16_t address, uint8_t value);

    // ROM related
    void load_rom(const std::vector<uint8_t>& rom_data); // since gb is 8bit architecture, gb roms opcodes are in uint_8
    void load_boot(const std::vector<uint8_t>& boot_data);
    void set_boot_enabled(bool on);

private:
    const std::unique_ptr<std::array<uint8_t, MEM_SIZE>> memory_array;

    bool boot_rom_enabled = false; // (0x0000 - 0x00FF) <- boot rom data
    const std::unique_ptr<std::array<uint8_t, BOOT_ROM_SIZE>> boot_array;

};

}

#endif //MEMORY_H