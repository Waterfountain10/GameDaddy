//
// Created by William Lafond on 2025-07-08.
//

#ifndef CPU_H
#define CPU_H
#include <array>
#include <cstdint>
#include <memory>

namespace GameBoy {
class Memory;

enum class Reg8;

class CPU {
public:
    CPU();

    void attach_memory(std::shared_ptr<Memory> mem);
    void reset_registers_fast(); // fake simulated for development purposes
    // void reset_registers_auth(); // authentic power-on boot for registers

    int step();

    uint8_t  get_register_at(Reg8 reg) const;
    uint16_t get_sp() const {
        return sp_;
    }
    uint16_t get_pc() const {
        return pc_;
    }
    bool halted() const {
        return halted_;
    }

    void set_register(Reg8 reg, uint8_t value);
    void set_sp(uint16_t value) {
        sp_ = value;
    }
    void set_pc(uint16_t value) {
        pc_ = value;
    }

private:
    // CPU 8-bit registers
    uint8_t  a_, f_; // Accumulator and Flag
    uint8_t  b_, c_; // BC - register
    uint8_t  d_, e_; // DE - register
    uint8_t  h_, l_; // HL - register
    uint16_t sp_;    // Stack Pointer
    uint16_t pc_;    // Program Counter
    bool     halted_ = false;

    // Flags for f_
    enum Flag {
        z = 1 << 7, // Zero Flag is 7th bit
        n = 1 << 6, // Substract Flag (BCD)
        h = 1 << 5, // Half-Carry Flag (BCD)
        c = 1 << 4  // Carry Flag
                    // rest = 0 (lower 4 bit)
    };

    std::shared_ptr<Memory> memory_;

    using Handler = int (CPU::*)(uint8_t opcode);
    static const std::array<Handler, 256>& opcode_table_();
    static const std::array<Handler, 256>& cb_table_();

    uint8_t  read8(uint16_t addr) const;
    void     write8(uint16_t addr, uint8_t value);
    uint8_t  fetch8();
    uint16_t fetch16();

    uint16_t bc() const;
    uint16_t de() const;
    uint16_t hl() const;
    uint16_t af() const;
    void     set_bc(uint16_t value);
    void     set_de(uint16_t value);
    void     set_hl(uint16_t value);
    void     set_af(uint16_t value);

    uint8_t  read_reg(int index) const;
    void     write_reg(int index, uint8_t value);
    uint16_t read_pair(int index) const;
    void     write_pair(int index, uint16_t value);

    bool get_flag(Flag flag) const;
    void set_flag(Flag flag, bool on);
    void set_znhc(bool zero, bool sub, bool half, bool carry);

    void     push16(uint16_t value);
    uint16_t pop16();

    uint8_t inc8(uint8_t value);
    uint8_t dec8(uint8_t value);
    void    add_a(uint8_t value);
    void    adc_a(uint8_t value);
    void    sub_a(uint8_t value);
    void    sbc_a(uint8_t value);
    void    and_a(uint8_t value);
    void    xor_a(uint8_t value);
    void    or_a(uint8_t value);
    void    cp_a(uint8_t value);

    int op_unimplemented(uint8_t opcode);
    int op_nop(uint8_t opcode);
    int op_halt(uint8_t opcode);
    int op_ld_r_n(uint8_t opcode);
    int op_ld_r_r(uint8_t opcode);
    int op_inc_r(uint8_t opcode);
    int op_dec_r(uint8_t opcode);
    int op_ld_rr_nn(uint8_t opcode);
    int op_inc_rr(uint8_t opcode);
    int op_dec_rr(uint8_t opcode);
    int op_add_hl_rr(uint8_t opcode);
    int op_alu_r(uint8_t opcode);
    int op_alu_n(uint8_t opcode);
    int op_jr(uint8_t opcode);
    int op_jp(uint8_t opcode);
    int op_call(uint8_t opcode);
    int op_ret(uint8_t opcode);
    int op_push(uint8_t opcode);
    int op_pop(uint8_t opcode);
    int op_ld_a_mem(uint8_t opcode);
    int op_ld_mem_a(uint8_t opcode);
    int op_ldh_a(uint8_t opcode);
    int op_ldh_mem(uint8_t opcode);
    int op_misc_a(uint8_t opcode);
    int op_sp_hl(uint8_t opcode);
    int op_cb(uint8_t opcode);
    int op_cb_rot(uint8_t opcode);
    int op_cb_bit(uint8_t opcode);
    int op_cb_res_set(uint8_t opcode);
};

enum class Reg8 { A, F, B, C, D, E, H, L };

} // namespace GameBoy

#endif // CPU_H
