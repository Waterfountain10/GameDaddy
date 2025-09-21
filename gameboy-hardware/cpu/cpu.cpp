//
// Created by William Lafond on 2025-07-08.
//

#include "cpu.h"

#include "../memory/memory.h"

#include <iostream>
#include <stdexcept>

namespace GameBoy {

CPU::CPU()
: a_{0}, f_{0}, b_{0}, c_{0}, d_{0}, e_{0}, h_{0}, l_{0}, pc_{0}, sp_{0} {}

void CPU::attach_memory(constexpr std::shared_ptr<Memory> mem) {
    memory_ = mem;
}

void CPU::reset_registers_fast() {
    // Set initial values according to original GameBoy (DMG) boot ROM specs
    // https://gbdev.io/pandocs/Power_Up_Sequence.html?highlight=boot#console-state-after-boot-rom-hand-off
    a_ = 0x01;
    f_ = 0xB0;
    b_ = 0x00;
    c_ = 0x13;
    d_ = 0x00;
    e_ = 0xD8;
    h_ = 0x01;
    l_ = 0x4D;
    pc_ = 0x0100;
    sp_ = 0xFFFE;
}

void CPU::reset_registers_auth() {
    a_ = 0;
    f_ = 0;
    b_ = 0;
    c_ = 0;
    d_ = 0;
    e_ = 0;
    h_ = 0;
    l_ = 0;
    pc_ = 0;
    sp_ = 0;
    sp_ = 0x0000;
    pc_ = 0x0000; // start executing at boot ROM
}

int CPU::step() {
    if (!memory_) throw std::runtime_error("There is no Memory attached to CPU");
    const uint16_t pc_before = pc_;
    uint8_t opcode = memory_->read_byte_at((pc_++)); // ++ after means read at pc_ then increment pc_

    // debug for temporary use
    std::cout << std::hex << "PC=" << pc_before << " OPC=" << (int)opcode << "\n";

    // decode/execute (skeleton)
    // TODO: remove this and use chip-8 switch table
    switch (opcode) {
        case 0x00: // NOP
            // do nothing
                return 4;
        default:
            // For now, just pretend it took 4 cycles
                return 4;
    }
}

uint8_t CPU::get_register_at(Reg8 reg) const {
    switch (reg) {
        case Reg8::A : return a_;
        case Reg8::F : return f_;
        case Reg8::B : return b_;
        case Reg8::C : return c_;
        case Reg8::D : return d_;
        case Reg8::E : return e_;
        case Reg8::H : return h_;
        case Reg8::L : return l_;
    default: return 0;
    }
}

void CPU::set_register(Reg8 reg, uint8_t value) {
    switch (reg) {
    case Reg8::A: a_ = value; break;
    case Reg8::F: f_ = value & 0xF0; break; // lower 4 bits are always 0
    case Reg8::B: b_ = value; break;
    case Reg8::C: c_ = value; break;
    case Reg8::D: d_ = value; break;
    case Reg8::E: e_ = value; break;
    case Reg8::H: h_ = value; break;
    case Reg8::L: l_ = value; break;
    default: throw std::invalid_argument("Invalid register");
    }
}

}