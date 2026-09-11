#include "gameboy/interrupts/interrupts.h"

#include <gtest/gtest.h>

TEST(Interrupts, ReadsAndWritesIeAndIfRegisters) {
    GameBoy::InterruptController interrupts;

    interrupts.write(0xFFFF, 0x1F);
    interrupts.write(0xFF0F, 0x04);

    EXPECT_EQ(interrupts.read(0xFFFF), 0x1F);
    EXPECT_EQ(interrupts.read(0xFF0F), 0xE4);
}

TEST(Interrupts, RequestsAndServicesHighestPriorityPendingInterrupt) {
    GameBoy::InterruptController interrupts;

    interrupts.write(0xFFFF, 0x05);
    interrupts.request(GameBoy::InterruptController::Timer);
    interrupts.request(GameBoy::InterruptController::VBlank);
    interrupts.set_ime(true);

    EXPECT_TRUE(interrupts.has_pending());
    EXPECT_TRUE(interrupts.can_service());
    EXPECT_EQ(interrupts.service_next(), 0x0040);
    EXPECT_FALSE(interrupts.ime());
    EXPECT_EQ(interrupts.read(0xFF0F) & 0x1F, 0x04);
}
