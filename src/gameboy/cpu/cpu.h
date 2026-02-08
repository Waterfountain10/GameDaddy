//
// Created by William Lafond on 2025-07-08.
//

#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <memory>

#endif //CPU_H

namespace GameBoy {

class Memory;

enum class Reg8;

class CPU {
public:
    CPU();

    void attach_memory(std::shared_ptr<Memory> mem);
    void reset_registers_fast(); // fake simulated for development purposes
    //void reset_registers_auth(); // authentic power-on boot for registers

    int step();

    // Getters
    uint8_t get_register_at(Reg8 reg) const;
    uint16_t get_sp() const {return sp_; }
    uint16_t get_pc() const { return pc_; }

    // Setters
    void set_register(Reg8 reg, uint8_t value);
    void set_sp(uint16_t value) { sp_ = value; }
    void set_pc(uint16_t value) { pc_ = value; }

private:
    // CPU 8-bit registers
    uint8_t a_, f_; // Accumulator and Flag
    uint8_t b_, c_; // BC - register
    uint8_t d_, e_; // DE - register
    uint8_t h_, l_; // HL - register
    uint16_t sp_;    // Stack Pointer
    uint16_t pc_;    // Program Counter

    // Flags for f_
    enum Flag {
        z = 1 << 7, // Zero Flag is 7th bit
        n = 1 << 6, // Substract Flag (BCD)
        h = 1 << 5, // Half-Carry Flag (BCD)
        c = 1 << 4 // Carry Flag
        // rest = 0 (lower 4 bit)
    };

    std::shared_ptr<Memory> memory_;

    // TODO: Implement opcode fetch-decode-execute
};


enum class Reg8 {
    A, F, B, C, D, E, H, L
};

}
