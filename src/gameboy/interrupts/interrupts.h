//
// Created by William Lafond on 2025-07-08.
//

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <cstdint>

namespace GameBoy {

class InterruptController {
public:
    enum Interrupt : uint8_t {
        VBlank = 0,
        Stat   = 1,
        Timer  = 2,
        Serial = 3,
        Joypad = 4,
    };

    uint8_t read(uint16_t addr) const;
    void    write(uint16_t addr, uint8_t value);

    void request(Interrupt interrupt);
    void set_ime(bool enabled);
    bool ime() const;

    bool     has_pending() const;
    bool     can_service() const;
    uint16_t service_next();

private:
    bool    ime_ = false;
    uint8_t ie_  = 0x00;
    uint8_t if_  = 0xE1;

    uint8_t pending_() const;
};

} // namespace GameBoy

#endif // INTERRUPTS_H
