cmake_minimum_required(VERSION 3.20)
project(gamedaddy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional quality-of-life
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(gamedaddy
        main.cpp
        platform-layer/platform.cpp
        platform-layer/platform.h
        platform-layer/display/impl/cli.cpp
        platform-layer/display/impl/cli.h
        platform-layer/display/impl/sdl_gui.cpp
        platform-layer/display/impl/sdl_gui.h
        platform-layer/display/display_interface.h

        gameboy-hardware/cpu/cpu.cpp
        gameboy-hardware/cpu/cpu.h
        gameboy-hardware/io/sound/sound.cpp
        gameboy-hardware/io/sound/sound.h
        gameboy-hardware/io/ppu/ppu.cpp
        gameboy-hardware/io/ppu/ppu.h
        gameboy-hardware/io/joypad/joypad.cpp
        gameboy-hardware/io/joypad/joypad.h
        gameboy-hardware/io/instructions/instructions.cpp
        gameboy-hardware/io/instructions/instructions.h
        gameboy-hardware/interrupts/interrupts.cpp
        gameboy-hardware/interrupts/interrupts.h
        gameboy-hardware/memory/memory.cpp
        gameboy-hardware/memory/memory.h

        # If these are unit tests with their own main(), DO NOT list them here.
        # test/cpu_test.cpp
        # test/ppu_test.cpp
        # test/instructions_test.cpp
        # test/interrupts_test.cpp
        # test/sound_test.cpp
        # test/cli_test.cpp
        # test/mbc_test.cpp
)

# SDL via imported targets from Homebrew packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
# If/when you add audio:
# find_package(SDL2_mixer CONFIG REQUIRED)

target_link_libraries(gamedaddy PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        # SDL2_mixer::SDL2_mixer
)

target_compile_features(gamedaddy PRIVATE cxx_std_20)
