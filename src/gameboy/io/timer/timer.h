#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

namespace GameBoy {

class Timer {
public:
    uint8_t read(uint16_t addr) const;
    void    write(uint16_t addr, uint8_t value);

    void tick(int cycles);
    bool consume_interrupt_request();

private:
    uint8_t div_  = 0x00;
    uint8_t tima_ = 0x00;
    uint8_t tma_  = 0x00;
    uint8_t tac_  = 0xF8;

    int  div_counter_         = 0;
    int  timer_counter_       = 0;
    bool interrupt_requested_ = false;

    bool timer_enabled_() const;
    int  timer_period_() const;
};

} // namespace GameBoy

#endif // TIMER_H
