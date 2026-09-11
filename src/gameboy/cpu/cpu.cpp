//
// Created by William Lafond on 2025-07-08.
//

#include "cpu.h"

#include "../memory/memory.h"

#include <stdexcept>

namespace GameBoy {

CPU::CPU() : a_{0}, f_{0}, b_{0}, c_{0}, d_{0}, e_{0}, h_{0}, l_{0}, pc_{0}, sp_{0} {}

void CPU::attach_memory(std::shared_ptr<Memory> mem) {
    memory_ = mem;
}

void CPU::reset_registers_fast() {
    // Set initial values according to original GameBoy (DMG) boot ROM specs
    // https://gbdev.io/pandocs/Power_Up_Sequence.html?highlight=boot#console-state-after-boot-rom-hand-off
    a_      = 0x01;
    f_      = 0xB0;
    b_      = 0x00;
    c_      = 0x13;
    d_      = 0x00;
    e_      = 0xD8;
    h_      = 0x01;
    l_      = 0x4D;
    pc_     = 0x0100;
    sp_     = 0xFFFE;
    halted_ = false;
}

const std::array<CPU::Handler, 256>& CPU::opcode_table_() {
    static const std::array<Handler, 256> table = [] {
        std::array<Handler, 256> t{};
        t.fill(&CPU::op_unimplemented);

        t[0x00] = &CPU::op_nop;
        t[0x76] = &CPU::op_halt;
        t[0xCB] = &CPU::op_cb;

        for (uint16_t op = 0x40; op <= 0x7F; ++op)
            if (op != 0x76)
                t[op] = &CPU::op_ld_r_r;
        for (uint8_t op : {0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E})
            t[op] = &CPU::op_ld_r_n;
        for (uint8_t op : {0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C})
            t[op] = &CPU::op_inc_r;
        for (uint8_t op : {0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D})
            t[op] = &CPU::op_dec_r;
        for (uint8_t op : {0x01, 0x11, 0x21, 0x31})
            t[op] = &CPU::op_ld_rr_nn;
        for (uint8_t op : {0x03, 0x13, 0x23, 0x33})
            t[op] = &CPU::op_inc_rr;
        for (uint8_t op : {0x0B, 0x1B, 0x2B, 0x3B})
            t[op] = &CPU::op_dec_rr;
        for (uint8_t op : {0x09, 0x19, 0x29, 0x39})
            t[op] = &CPU::op_add_hl_rr;
        for (uint16_t op = 0x80; op <= 0xBF; ++op)
            t[op] = &CPU::op_alu_r;
        for (uint8_t op : {0xC6, 0xCE, 0xD6, 0xDE, 0xE6, 0xEE, 0xF6, 0xFE})
            t[op] = &CPU::op_alu_n;
        for (uint8_t op : {0x18, 0x20, 0x28, 0x30, 0x38})
            t[op] = &CPU::op_jr;
        for (uint8_t op : {0xC3, 0xC2, 0xCA, 0xD2, 0xDA})
            t[op] = &CPU::op_jp;
        t[0xCD] = &CPU::op_call;
        for (uint8_t op : {0xC9, 0xC0, 0xC8, 0xD0, 0xD8})
            t[op] = &CPU::op_ret;
        for (uint8_t op : {0xC5, 0xD5, 0xE5, 0xF5})
            t[op] = &CPU::op_push;
        for (uint8_t op : {0xC1, 0xD1, 0xE1, 0xF1})
            t[op] = &CPU::op_pop;
        for (uint8_t op : {0x0A, 0x1A, 0x2A, 0x3A, 0xF0, 0xF2, 0xFA})
            t[op] = &CPU::op_ld_a_mem;
        for (uint8_t op : {0x02, 0x12, 0x22, 0x32, 0xE0, 0xE2, 0xEA, 0x08})
            t[op] = &CPU::op_ld_mem_a;
        for (uint8_t op : {0x07, 0x0F, 0x17, 0x1F, 0x27, 0x2F, 0x37, 0x3F})
            t[op] = &CPU::op_misc_a;
        for (uint8_t op : {0xE8, 0xF8, 0xF9})
            t[op] = &CPU::op_sp_hl;

        return t;
    }();
    return table;
}

const std::array<CPU::Handler, 256>& CPU::cb_table_() {
    static const std::array<Handler, 256> table = [] {
        std::array<Handler, 256> t{};
        for (uint16_t op = 0x00; op <= 0x3F; ++op)
            t[op] = &CPU::op_cb_rot;
        for (uint16_t op = 0x40; op <= 0x7F; ++op)
            t[op] = &CPU::op_cb_bit;
        for (uint16_t op = 0x80; op <= 0xFF; ++op)
            t[op] = &CPU::op_cb_res_set;
        return t;
    }();
    return table;
}

int CPU::step() {
    if (!memory_)
        throw std::runtime_error("There is no Memory attached to CPU");
    if (halted_)
        return 4;

    uint8_t opcode = fetch8();
    return (this->*opcode_table_()[opcode])(opcode);
}

uint8_t CPU::read8(uint16_t addr) const {
    return memory_->read_byte_at(addr);
}

void CPU::write8(uint16_t addr, uint8_t value) {
    memory_->write_byte_at(addr, value);
}

uint8_t CPU::fetch8() {
    return read8(pc_++);
}

uint16_t CPU::fetch16() {
    uint16_t lo = fetch8();
    return lo | (fetch8() << 8);
}

uint16_t CPU::bc() const {
    return (b_ << 8) | c_;
}
uint16_t CPU::de() const {
    return (d_ << 8) | e_;
}
uint16_t CPU::hl() const {
    return (h_ << 8) | l_;
}
uint16_t CPU::af() const {
    return (a_ << 8) | f_;
}
void CPU::set_bc(uint16_t value) {
    b_ = value >> 8;
    c_ = value & 0xFF;
}
void CPU::set_de(uint16_t value) {
    d_ = value >> 8;
    e_ = value & 0xFF;
}
void CPU::set_hl(uint16_t value) {
    h_ = value >> 8;
    l_ = value & 0xFF;
}
void CPU::set_af(uint16_t value) {
    a_ = value >> 8;
    f_ = value & 0xF0;
}

uint8_t CPU::read_reg(int index) const {
    switch (index) {
    case 0:
        return b_;
    case 1:
        return c_;
    case 2:
        return d_;
    case 3:
        return e_;
    case 4:
        return h_;
    case 5:
        return l_;
    case 6:
        return read8(hl());
    case 7:
        return a_;
    default:
        return 0xFF;
    }
}

void CPU::write_reg(int index, uint8_t value) {
    switch (index) {
    case 0:
        b_ = value;
        break;
    case 1:
        c_ = value;
        break;
    case 2:
        d_ = value;
        break;
    case 3:
        e_ = value;
        break;
    case 4:
        h_ = value;
        break;
    case 5:
        l_ = value;
        break;
    case 6:
        write8(hl(), value);
        break;
    case 7:
        a_ = value;
        break;
    }
}

uint16_t CPU::read_pair(int index) const {
    switch (index) {
    case 0:
        return bc();
    case 1:
        return de();
    case 2:
        return hl();
    case 3:
        return sp_;
    default:
        return 0;
    }
}

void CPU::write_pair(int index, uint16_t value) {
    switch (index) {
    case 0:
        set_bc(value);
        break;
    case 1:
        set_de(value);
        break;
    case 2:
        set_hl(value);
        break;
    case 3:
        sp_ = value;
        break;
    }
}

bool CPU::get_flag(Flag flag) const {
    return (f_ & flag) != 0;
}

void CPU::set_flag(Flag flag, bool on) {
    if (on)
        f_ |= flag;
    else
        f_ &= ~flag;
    f_ &= 0xF0;
}

void CPU::set_znhc(bool zero, bool sub, bool half, bool carry) {
    f_ = (zero ? z : 0) | (sub ? n : 0) | (half ? h : 0) | (carry ? c : 0);
}

void CPU::push16(uint16_t value) {
    write8(--sp_, value >> 8);
    write8(--sp_, value & 0xFF);
}

uint16_t CPU::pop16() {
    uint16_t lo = read8(sp_++);
    return lo | (read8(sp_++) << 8);
}

uint8_t CPU::inc8(uint8_t value) {
    uint8_t out = value + 1;
    set_flag(z, out == 0);
    set_flag(n, false);
    set_flag(h, (value & 0x0F) == 0x0F);
    return out;
}

uint8_t CPU::dec8(uint8_t value) {
    uint8_t out = value - 1;
    set_flag(z, out == 0);
    set_flag(n, true);
    set_flag(h, (value & 0x0F) == 0);
    return out;
}

void CPU::add_a(uint8_t value) {
    uint16_t out = a_ + value;
    set_znhc((out & 0xFF) == 0, false, ((a_ & 0x0F) + (value & 0x0F)) > 0x0F, out > 0xFF);
    a_ = out & 0xFF;
}

void CPU::adc_a(uint8_t value) {
    uint8_t  carry = get_flag(c) ? 1 : 0;
    uint16_t out   = a_ + value + carry;
    set_znhc((out & 0xFF) == 0, false, ((a_ & 0x0F) + (value & 0x0F) + carry) > 0x0F, out > 0xFF);
    a_ = out & 0xFF;
}

void CPU::sub_a(uint8_t value) {
    set_znhc(a_ == value, true, (a_ & 0x0F) < (value & 0x0F), a_ < value);
    a_ -= value;
}

void CPU::sbc_a(uint8_t value) {
    uint8_t  carry = get_flag(c) ? 1 : 0;
    uint16_t out   = a_ - value - carry;
    set_znhc((out & 0xFF) == 0, true, (a_ & 0x0F) < ((value & 0x0F) + carry), a_ < value + carry);
    a_ = out & 0xFF;
}

void CPU::and_a(uint8_t value) {
    a_ &= value;
    set_znhc(a_ == 0, false, true, false);
}

void CPU::xor_a(uint8_t value) {
    a_ ^= value;
    set_znhc(a_ == 0, false, false, false);
}

void CPU::or_a(uint8_t value) {
    a_ |= value;
    set_znhc(a_ == 0, false, false, false);
}

void CPU::cp_a(uint8_t value) {
    set_znhc(a_ == value, true, (a_ & 0x0F) < (value & 0x0F), a_ < value);
}

int CPU::op_unimplemented(uint8_t opcode) {
    throw std::runtime_error("Unsupported CPU opcode");
}

int CPU::op_nop(uint8_t opcode) {
    (void)opcode;
    return 4;
}

int CPU::op_halt(uint8_t opcode) {
    (void)opcode;
    halted_ = true;
    return 4;
}

int CPU::op_ld_r_n(uint8_t opcode) {
    int r = (opcode >> 3) & 0x07;
    write_reg(r, fetch8());
    return r == 6 ? 12 : 8;
}

int CPU::op_ld_r_r(uint8_t opcode) {
    int dst = (opcode >> 3) & 0x07;
    int src = opcode & 0x07;
    write_reg(dst, read_reg(src));
    return (dst == 6 || src == 6) ? 8 : 4;
}

int CPU::op_inc_r(uint8_t opcode) {
    int r = (opcode >> 3) & 0x07;
    write_reg(r, inc8(read_reg(r)));
    return r == 6 ? 12 : 4;
}

int CPU::op_dec_r(uint8_t opcode) {
    int r = (opcode >> 3) & 0x07;
    write_reg(r, dec8(read_reg(r)));
    return r == 6 ? 12 : 4;
}

int CPU::op_ld_rr_nn(uint8_t opcode) {
    write_pair((opcode >> 4) & 0x03, fetch16());
    return 12;
}

int CPU::op_inc_rr(uint8_t opcode) {
    int p = (opcode >> 4) & 0x03;
    write_pair(p, read_pair(p) + 1);
    return 8;
}

int CPU::op_dec_rr(uint8_t opcode) {
    int p = (opcode >> 4) & 0x03;
    write_pair(p, read_pair(p) - 1);
    return 8;
}

int CPU::op_add_hl_rr(uint8_t opcode) {
    uint16_t rhs = read_pair((opcode >> 4) & 0x03);
    uint32_t out = hl() + rhs;
    set_flag(n, false);
    set_flag(h, ((hl() & 0x0FFF) + (rhs & 0x0FFF)) > 0x0FFF);
    set_flag(c, out > 0xFFFF);
    set_hl(out & 0xFFFF);
    return 8;
}

int CPU::op_alu_r(uint8_t opcode) {
    uint8_t value = read_reg(opcode & 0x07);
    switch ((opcode >> 3) & 0x07) {
    case 0:
        add_a(value);
        break;
    case 1:
        adc_a(value);
        break;
    case 2:
        sub_a(value);
        break;
    case 3:
        sbc_a(value);
        break;
    case 4:
        and_a(value);
        break;
    case 5:
        xor_a(value);
        break;
    case 6:
        or_a(value);
        break;
    case 7:
        cp_a(value);
        break;
    }
    return (opcode & 0x07) == 6 ? 8 : 4;
}

int CPU::op_alu_n(uint8_t opcode) {
    uint8_t value = fetch8();
    if (opcode == 0xC6)
        add_a(value);
    else if (opcode == 0xCE)
        adc_a(value);
    else if (opcode == 0xD6)
        sub_a(value);
    else if (opcode == 0xDE)
        sbc_a(value);
    else if (opcode == 0xE6)
        and_a(value);
    else if (opcode == 0xEE)
        xor_a(value);
    else if (opcode == 0xF6)
        or_a(value);
    else
        cp_a(value);
    return 8;
}

int CPU::op_jr(uint8_t opcode) {
    int8_t offset = fetch8();
    bool   take   = opcode == 0x18 || (opcode == 0x20 && !get_flag(z)) ||
                    (opcode == 0x28 && get_flag(z)) || (opcode == 0x30 && !get_flag(c)) ||
                    (opcode == 0x38 && get_flag(c));
    if (take) {
        pc_ += offset;
        return 12;
    }
    return 8;
}

int CPU::op_jp(uint8_t opcode) {
    uint16_t addr = fetch16();
    bool     take = opcode == 0xC3 || (opcode == 0xC2 && !get_flag(z)) ||
                    (opcode == 0xCA && get_flag(z)) || (opcode == 0xD2 && !get_flag(c)) ||
                    (opcode == 0xDA && get_flag(c));
    if (take) {
        pc_ = addr;
        return 16;
    }
    return 12;
}

int CPU::op_call(uint8_t opcode) {
    (void)opcode;
    uint16_t addr = fetch16();
    push16(pc_);
    pc_ = addr;
    return 24;
}

int CPU::op_ret(uint8_t opcode) {
    bool take = opcode == 0xC9 || (opcode == 0xC0 && !get_flag(z)) ||
                (opcode == 0xC8 && get_flag(z)) || (opcode == 0xD0 && !get_flag(c)) ||
                (opcode == 0xD8 && get_flag(c));
    if (take) {
        pc_ = pop16();
        return opcode == 0xC9 ? 16 : 20;
    }
    return 8;
}

int CPU::op_push(uint8_t opcode) {
    uint16_t value[] = {bc(), de(), hl(), af()};
    push16(value[(opcode >> 4) & 0x03]);
    return 16;
}

int CPU::op_pop(uint8_t opcode) {
    uint16_t value = pop16();
    switch ((opcode >> 4) & 0x03) {
    case 0:
        set_bc(value);
        break;
    case 1:
        set_de(value);
        break;
    case 2:
        set_hl(value);
        break;
    case 3:
        set_af(value);
        break;
    }
    return 12;
}

int CPU::op_ld_a_mem(uint8_t opcode) {
    if (opcode == 0x0A)
        a_ = read8(bc());
    else if (opcode == 0x1A)
        a_ = read8(de());
    else if (opcode == 0x2A) {
        a_ = read8(hl());
        set_hl(hl() + 1);
    } else if (opcode == 0x3A) {
        a_ = read8(hl());
        set_hl(hl() - 1);
    } else if (opcode == 0xF0)
        a_ = read8(0xFF00 + fetch8());
    else if (opcode == 0xF2)
        a_ = read8(0xFF00 + c_);
    else
        a_ = read8(fetch16());
    return opcode == 0xFA ? 16 : (opcode == 0xF0 ? 12 : 8);
}

int CPU::op_ld_mem_a(uint8_t opcode) {
    if (opcode == 0x02)
        write8(bc(), a_);
    else if (opcode == 0x12)
        write8(de(), a_);
    else if (opcode == 0x22) {
        write8(hl(), a_);
        set_hl(hl() + 1);
    } else if (opcode == 0x32) {
        write8(hl(), a_);
        set_hl(hl() - 1);
    } else if (opcode == 0xE0)
        write8(0xFF00 + fetch8(), a_);
    else if (opcode == 0xE2)
        write8(0xFF00 + c_, a_);
    else if (opcode == 0xEA)
        write8(fetch16(), a_);
    else {
        uint16_t addr = fetch16();
        write8(addr, sp_ & 0xFF);
        write8(addr + 1, sp_ >> 8);
    }
    return opcode == 0xEA || opcode == 0x08 ? 16 : (opcode == 0xE0 ? 12 : 8);
}

int CPU::op_misc_a(uint8_t opcode) {
    if (opcode == 0x07) {
        bool carry = a_ & 0x80;
        a_         = (a_ << 1) | carry;
        set_znhc(false, false, false, carry);
    } else if (opcode == 0x0F) {
        bool carry = a_ & 1;
        a_         = (a_ >> 1) | (carry << 7);
        set_znhc(false, false, false, carry);
    } else if (opcode == 0x17) {
        bool old = get_flag(c);
        bool ca  = a_ & 0x80;
        a_       = (a_ << 1) | old;
        set_znhc(false, false, false, ca);
    } else if (opcode == 0x1F) {
        bool old = get_flag(c);
        bool ca  = a_ & 1;
        a_       = (a_ >> 1) | (old << 7);
        set_znhc(false, false, false, ca);
    } else if (opcode == 0x2F) {
        a_ ^= 0xFF;
        set_flag(n, true);
        set_flag(h, true);
    } else if (opcode == 0x37) {
        set_flag(n, false);
        set_flag(h, false);
        set_flag(c, true);
    } else if (opcode == 0x3F) {
        set_flag(n, false);
        set_flag(h, false);
        set_flag(c, !get_flag(c));
    }
    return 4;
}

int CPU::op_sp_hl(uint8_t opcode) {
    if (opcode == 0xF9) {
        sp_ = hl();
        return 8;
    }

    int8_t   offset = fetch8();
    uint16_t old    = sp_;
    uint16_t out    = sp_ + offset;
    set_znhc(false, false, ((old & 0x0F) + (offset & 0x0F)) > 0x0F,
             ((old & 0xFF) + (offset & 0xFF)) > 0xFF);
    if (opcode == 0xE8)
        sp_ = out;
    else
        set_hl(out);
    return opcode == 0xE8 ? 16 : 12;
}

int CPU::op_cb(uint8_t opcode) {
    (void)opcode;
    uint8_t cb = fetch8();
    return (this->*cb_table_()[cb])(cb);
}

int CPU::op_cb_rot(uint8_t opcode) {
    int     r     = opcode & 0x07;
    uint8_t value = read_reg(r);
    bool    carry = false;

    switch ((opcode >> 3) & 0x07) {
    case 0:
        carry = value & 0x80;
        value = (value << 1) | carry;
        break;
    case 1:
        carry = value & 1;
        value = (value >> 1) | (carry << 7);
        break;
    case 2: {
        bool old = get_flag(c);
        carry    = value & 0x80;
        value    = (value << 1) | old;
        break;
    }
    case 3: {
        bool old = get_flag(c);
        carry    = value & 1;
        value    = (value >> 1) | (old << 7);
        break;
    }
    case 4:
        carry = value & 0x80;
        value <<= 1;
        break;
    case 5:
        carry = value & 1;
        value = (value >> 1) | (value & 0x80);
        break;
    case 6:
        value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
        break;
    case 7:
        carry = value & 1;
        value >>= 1;
        break;
    }

    write_reg(r, value);
    set_znhc(value == 0, false, false, carry);
    return r == 6 ? 16 : 8;
}

int CPU::op_cb_bit(uint8_t opcode) {
    int     r     = opcode & 0x07;
    uint8_t value = read_reg(r);
    uint8_t bit   = (opcode >> 3) & 0x07;
    set_znhc((value & (1 << bit)) == 0, false, true, get_flag(c));
    return r == 6 ? 12 : 8;
}

int CPU::op_cb_res_set(uint8_t opcode) {
    int     r     = opcode & 0x07;
    uint8_t value = read_reg(r);
    uint8_t bit   = (opcode >> 3) & 0x07;
    if (opcode < 0xC0)
        value &= ~(1 << bit);
    else
        value |= (1 << bit);
    write_reg(r, value);
    return r == 6 ? 16 : 8;
}

uint8_t CPU::get_register_at(Reg8 reg) const {
    switch (reg) {
    case Reg8::A:
        return a_;
    case Reg8::F:
        return f_;
    case Reg8::B:
        return b_;
    case Reg8::C:
        return c_;
    case Reg8::D:
        return d_;
    case Reg8::E:
        return e_;
    case Reg8::H:
        return h_;
    case Reg8::L:
        return l_;
    default:
        return 0;
    }
}

void CPU::set_register(Reg8 reg, uint8_t value) {
    switch (reg) {
    case Reg8::A:
        a_ = value;
        break;
    case Reg8::F:
        f_ = value & 0xF0;
        break; // lower 4 bits are always 0
    case Reg8::B:
        b_ = value;
        break;
    case Reg8::C:
        c_ = value;
        break;
    case Reg8::D:
        d_ = value;
        break;
    case Reg8::E:
        e_ = value;
        break;
    case Reg8::H:
        h_ = value;
        break;
    case Reg8::L:
        l_ = value;
        break;
    default:
        throw std::invalid_argument("Invalid register");
    }
}

} // namespace GameBoy
