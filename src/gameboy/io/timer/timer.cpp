#include "timer.h"

namespace GameBoy {

uint8_t Timer::read(uint16_t addr) const {
    switch (addr) {
    case 0xFF04:
        return div_;
    case 0xFF05:
        return tima_;
    case 0xFF06:
        return tma_;
    case 0xFF07:
        return tac_ | 0xF8;
    default:
        return 0xFF;
    }
}

void Timer::write(uint16_t addr, uint8_t value) {
    switch (addr) {
    case 0xFF04:
        div_         = 0;
        div_counter_ = 0;
        break;
    case 0xFF05:
        tima_ = value;
        break;
    case 0xFF06:
        tma_ = value;
        break;
    case 0xFF07:
        tac_ = 0xF8 | (value & 0x07);
        break;
    }
}

void Timer::tick(int cycles) {
    div_counter_ += cycles;
    while (div_counter_ >= 256) {
        div_counter_ -= 256;
        ++div_;
    }

    if (!timer_enabled_())
        return;

    timer_counter_ += cycles;
    while (timer_counter_ >= timer_period_()) {
        timer_counter_ -= timer_period_();
        if (tima_ == 0xFF) {
            tima_                = tma_;
            interrupt_requested_ = true;
        } else {
            ++tima_;
        }
    }
}

bool Timer::consume_interrupt_request() {
    bool requested       = interrupt_requested_;
    interrupt_requested_ = false;
    return requested;
}

bool Timer::timer_enabled_() const {
    return (tac_ & 0x04) != 0;
}

int Timer::timer_period_() const {
    switch (tac_ & 0x03) {
    case 0x00:
        return 1024;
    case 0x01:
        return 16;
    case 0x02:
        return 64;
    case 0x03:
        return 256;
    default:
        return 1024;
    }
}

} // namespace GameBoy
