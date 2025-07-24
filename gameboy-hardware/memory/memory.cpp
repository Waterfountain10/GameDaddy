#include "memory.h"

#include <vector>
//
// Created by William Lafond on 2025-07-22.
//
namespace GameBoy {

Memory::Memory() :
    memory_array(std::make_unique<std::array<uint8_t, MEM_SIZE>>()) {
    memory_array->fill(0);
}

uint8_t Memory::read_byte_at(uint16_t address) {
    uint8_t byte = memory_array->at(address);
    return byte;
}

void Memory::write_byte_at(uint16_t address, uint8_t value) {
    memory_array->at(address) = value;
}

void Memory::load_rom(const std::vector<uint8_t>& rom_data) {
    size_t load_size = std::min(rom_data.size(), size_t(0x8000)); // NOT necessarily rom size, we take the first 32KB for MBC
    std::copy(
        rom_data.begin(),
        rom_data.begin() + load_size,
        memory_array->begin()
        );
}
}

