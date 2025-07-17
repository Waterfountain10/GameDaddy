//
// Created by William Lafond on 2025-07-08.
//

#include "cpu.h"
#include <stdexcept>

namespace GameBoy {

CPU::CPU() : a_{0}, f_{0}, b_{0}, c_{0}, d_{0}, e_{0}, h_{0}, l_{0}
{
    reset_registers();
}
void CPU::reset_registers() {
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

uint8_t CPU::get_register_at(Reg8 reg) const {
    switch (reg) {
        case "A" : return a_;
        case "F" : return f_;
        case "B" : return b_;
        case "C" : return c_;
        case "D" : return d_;
        case "E" : return e_;
        case "H" : return h_;
        case "L" : return l_;
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