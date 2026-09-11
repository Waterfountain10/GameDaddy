#include "gameboy/cpu/cpu.h"
#include "gameboy/memory/memory.h"

#include <gtest/gtest.h>
#include <memory>

namespace {
std::shared_ptr<GameBoy::Memory> memory_with(std::initializer_list<uint8_t> bytes) {
    auto     mem  = std::make_shared<GameBoy::Memory>();
    uint16_t addr = 0x0100;
    for (uint8_t byte : bytes)
        mem->write_byte_at(addr++, byte);
    return mem;
}
} // namespace

TEST(CPU, ExecutesLoadsAluAndRelativeJump) {
    GameBoy::CPU cpu;
    auto         mem = memory_with({0x3E, 0x01, 0x06, 0x02, 0x80, 0x20, 0x02, 0x3E, 0xFF, 0x00});
    cpu.attach_memory(mem);
    cpu.reset_registers_fast();

    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();

    EXPECT_EQ(cpu.get_register_at(GameBoy::Reg8::A), 0x03);
    EXPECT_EQ(cpu.get_pc(), 0x010A);
}

TEST(CPU, ExecutesCallReturnAndStackOps) {
    GameBoy::CPU cpu;
    auto mem = memory_with({0xCD, 0x08, 0x01, 0x3E, 0x22, 0x00, 0x00, 0x00, 0x3E, 0x77, 0xC9});
    cpu.attach_memory(mem);
    cpu.reset_registers_fast();

    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();

    EXPECT_EQ(cpu.get_register_at(GameBoy::Reg8::A), 0x22);
}

TEST(CPU, ExecutesCbBitSetAndReset) {
    GameBoy::CPU cpu;
    auto         mem = memory_with({0x06, 0x00, 0xCB, 0xC0, 0xCB, 0x40, 0xCB, 0x80});
    cpu.attach_memory(mem);
    cpu.reset_registers_fast();

    cpu.step();
    cpu.step();
    cpu.step();
    EXPECT_EQ(cpu.get_register_at(GameBoy::Reg8::F) & 0x80, 0x00);
    cpu.step();
    EXPECT_EQ(cpu.get_register_at(GameBoy::Reg8::B), 0x00);
}
