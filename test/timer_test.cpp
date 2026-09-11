#include "gameboy/io/timer/timer.h"

#include <gtest/gtest.h>

TEST(Timer, DivIncrementsAndResetsOnWrite) {
    GameBoy::Timer timer;

    timer.tick(255);
    EXPECT_EQ(timer.read(0xFF04), 0x00);

    timer.tick(1);
    EXPECT_EQ(timer.read(0xFF04), 0x01);

    timer.write(0xFF04, 0xFF);
    EXPECT_EQ(timer.read(0xFF04), 0x00);
}

TEST(Timer, TimaIncrementsAtSelectedFrequencyWhenEnabled) {
    GameBoy::Timer timer;

    timer.write(0xFF07, 0x05); // enable, 16-cycle period
    timer.tick(15);
    EXPECT_EQ(timer.read(0xFF05), 0x00);

    timer.tick(1);
    EXPECT_EQ(timer.read(0xFF05), 0x01);
}

TEST(Timer, OverflowReloadsModuloAndRequestsInterrupt) {
    GameBoy::Timer timer;

    timer.write(0xFF06, 0x42);
    timer.write(0xFF05, 0xFF);
    timer.write(0xFF07, 0x05);

    timer.tick(16);

    EXPECT_EQ(timer.read(0xFF05), 0x42);
    EXPECT_TRUE(timer.consume_interrupt_request());
    EXPECT_FALSE(timer.consume_interrupt_request());
}
