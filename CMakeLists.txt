cmake_minimum_required(VERSION 3.31)
project(gamedaddy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_SYSTEM MATCHES Windows)
    message(STATUS "Target system is Windows")
endif()

if(CMAKE_SYSTEM MATCHES APPLE)
    message(STATUS "Target system is MacOS")
endif()

if(CMAKE_SYSTEM MATCHES Linux)
    message(STATUS "Target system runs Linux")
endif()

add_executable(gamedaddy main.cpp
        platform-layer/platform.cpp
        platform-layer/platform.h
        platform-layer/display/impl/cli.cpp
        platform-layer/display/impl/cli.h
        gameboy-hardware/cpu/cpu.cpp
        gameboy-hardware/cpu/cpu.h
        gameboy-hardware/io/sound/sound.cpp
        gameboy-hardware/io/ppu/ppu.cpp
        gameboy-hardware/io/joypad/joypad.cpp
        gameboy-hardware/io/sound/sound.h
        gameboy-hardware/io/ppu/ppu.h
        gameboy-hardware/io/joypad/joypad.h
        gameboy-hardware/mbc/mbc.cpp
        gameboy-hardware/mbc/mbc.h
        gameboy-hardware/io/instructions/instructions.h
        gameboy-hardware/io/instructions/instructions.cpp
        gameboy-hardware/interrupts/interrupts.cpp
        gameboy-hardware/interrupts/interrupts.h
        test/cpu_test.cpp
        test/ppu_test.cpp
        test/instructions_test.cpp
        test/interrupts_test.cpp
        test/sound_test.cpp
        test/cli_test.cpp
        test/mbc_test.cpp
        platform-layer/display/impl/sdl_gui.cpp
        platform-layer/display/impl/sdl_gui.h
        platform-layer/display/display_interface.h
        gameboy-hardware/memory/memory.cpp
        gameboy-hardware/memory/memory.h
)


find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image REQUIRED)

target_compile_features(gamedaddy
        PRIVATE
        cxx_std_20
)

## For Mac (ARM) with brew-installed SDL2_image
if(APPLE)
    if(EXISTS "/opt/homebrew/Cellar/sdl2_image")
        include_directories("/opt/homebrew/include")
        target_link_directories(gamedaddy PRIVATE "/opt/homebrew/lib")
        target_link_libraries(gamedaddy PRIVATE SDL2 SDL2_image SDL2_mixer)
    endif()
endif()

## For Linux/MacOS_x86 with package installed packages
if(UNIX)
    # For UNIX operating systems with apt install
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(gamedaddy
            PRIVATE
            ${SDL2_LIBRARIES}
            SDL2::SDL2
            SDL2::SDL2main
    ) # links all libraries to our gameboy
    target_include_directories(gamedaddy
            PRIVATE
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_IMAGE_INCLUDE_DIRS}
            ${SDL2_MIXER_INCLUDE_DIRS}
    ) # tells CLion where the headers are)
endif()

## For Cross-Compiling to Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)          # avoid mixing in shared libs later

    message(STATUS "Configuring Windows target")

    target_link_options(gamedaddy
            PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )

    target_link_libraries(gamedaddy
            PRIVATE
            ${SDL2_LIBRARIES}
            SDL2::SDL2
            SDL2::SDL2main
            SDL2_image::SDL2_image
    )

    target_include_directories(gamedaddy
            PRIVATE
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_IMAGE_INCLUDE_DIRS}
            ${SDL2_MIXER_INCLUDE_DIRS}
    )

    # After every build, copy the DLLs next to the .exe
    add_custom_command(
            TARGET gamedaddy POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:gamedaddy>
            $<TARGET_FILE_DIR:gamedaddy>
            COMMAND_EXPAND_LISTS
    )

endif()

install(TARGETS gamedaddy
        RUNTIME DESTINATION .
        COMPONENT Runtime)

install(DIRECTORY tests/ DESTINATION roms COMPONENT Runtime)
install(FILES LICENSE README.md DESTINATION . COMPONENT Runtime)

## Packaging metadata
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "GameDaddy")
set(CPACK_PACKAGE_VERSION  "1.0.0")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")
include(CPack)
