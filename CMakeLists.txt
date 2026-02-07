cmake_minimum_required(VERSION 3.20)
project(gamedaddy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional quality-of-life
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(gamedaddy
        src/main.cpp

        src/_platform/platform.cpp
        src/_platform/platform.h
        src/_platform/display/impl/cli.cpp
        src/_platform/display/impl/cli.h
        src/_platform/display/impl/sdl_gui.cpp
        src/_platform/display/impl/sdl_gui.h
        src/_platform/display/display_interface.h

        src/gameboy/cpu/cpu.cpp
        src/gameboy/cpu/cpu.h
        src/gameboy/io/sound/sound.cpp
        src/gameboy/io/sound/sound.h
        src/gameboy/io/ppu/ppu.cpp
        src/gameboy/io/ppu/ppu.h
        src/gameboy/io/joypad/joypad.cpp
        src/gameboy/io/joypad/joypad.h
        src/gameboy/io/instructions/instructions.cpp
        src/gameboy/io/instructions/instructions.h
        src/gameboy/interrupts/interrupts.cpp
        src/gameboy/interrupts/interrupts.h
        src/gameboy/memory/memory.cpp
        src/gameboy/memory/memory.h

        src/cartridge/rom/rom-validation.cpp
        src/cartridge/rom/rom-validation.h

        include/units.h
)

# SDL via imported targets from Homebrew packages
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
# find_package(SDL2_mixer CONFIG REQUIRED) for audio

target_link_libraries(gamedaddy PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        #SDL2_mixer::SDL2_mixer
)

target_compile_features(gamedaddy PRIVATE cxx_std_20)
