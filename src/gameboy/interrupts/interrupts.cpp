//
// Created by William Lafond on 2025-07-08.
//

#include "interrupts.h"

namespace GameBoy {

uint8_t InterruptController::read(uint16_t addr) const {
    if (addr == 0xFFFF)
        return ie_;
    if (addr == 0xFF0F)
        return if_ | 0xE0;
    return 0xFF;
}

void InterruptController::write(uint16_t addr, uint8_t value) {
    if (addr == 0xFFFF)
        ie_ = value & 0x1F;
    else if (addr == 0xFF0F)
        if_ = 0xE0 | (value & 0x1F);
}

void InterruptController::request(Interrupt interrupt) {
    if_ = 0xE0 | (if_ & 0x1F) | (1 << interrupt);
}

void InterruptController::set_ime(bool enabled) {
    ime_ = enabled;
}

bool InterruptController::ime() const {
    return ime_;
}

uint8_t InterruptController::pending_() const {
    return ie_ & if_ & 0x1F;
}

bool InterruptController::has_pending() const {
    return pending_() != 0;
}

bool InterruptController::can_service() const {
    return ime_ && has_pending();
}

uint16_t InterruptController::service_next() {
    if (!can_service())
        return 0x0000;

    for (uint8_t bit = 0; bit < 5; ++bit) {
        if ((pending_() & (1 << bit)) != 0) {
            ime_ = false;
            if_  = 0xE0 | (if_ & ~(1 << bit));
            return static_cast<uint16_t>(0x0040 + bit * 0x0008);
        }
    }

    return 0x0000;
}

} // namespace GameBoy
