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
    if (boot_rom_enabled && address < 0x0100) {
        return boot_array->at(address);
    }
    return memory_array->at(address);
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

void Memory::load_boot(const std::vector<uint8_t>& boot_data) {
    size_t load_size = std::min(boot_data.size(), boot_array->size());
    std::copy(
            boot_data.begin(),
            boot_data.begin() + load_size,
            boot_array->begin()
    );
    boot_rom_enabled = true;
}

void Memory::set_boot_enabled(bool on) {
    boot_rom_enabled = on;
}


}

