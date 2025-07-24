//
// Created by William Lafond on 2025-07-22.
//

#ifndef MEMORY_H
#define MEMORY_H
#include <array>
#include <cstdint>

namespace GameBoy {

class Memory {
public:
    // CORE
    Memory();
    static constexpr size_t MEM_SIZE = 0x10000; // 65535 bytes + 1 byte (or 64 KB)
    uint8_t read_byte_at(uint16_t address);
    void write_byte_at(uint16_t address, uint8_t value);

    // ROM related
    void load_rom(const std::vector<uint8_t>& rom_data); // since gb is 8bit architecture, gb roms opcodes are in uint_8
    // TODO: add the ROM boot-rom boolean
    // TODO: load-bootrom or something
private:
    const std::unique_ptr<std::array<uint8_t, MEM_SIZE>> memory_array;
};

}

#endif //MEMORY_H