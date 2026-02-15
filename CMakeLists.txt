cmake_minimum_required(VERSION 3.20)
project(gamedaddy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------
# GoogleTest
# ---------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# ---------------------------
# Core Emulator Library (public since we testing this)
#    - CPU
#    - Memory
#    - Cartridge
#    - MBC
#    - PPU
#    - Sound
#    - Interrupts
# ---------------------------
file(GLOB_RECURSE CORE_SRC
    src/gameboy/**/*.cpp
    src/cartridge/**/*.cpp
)

add_library(gamedaddy_core ${CORE_SRC})

target_include_directories(gamedaddy_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(gamedaddy_core PUBLIC cxx_std_20)

# ---------------------------
# Main Executable (private)
#     - main.cpp
#     - platform.cpp
#     - sdl_gui.cpp
#     - links SDL
# ---------------------------
file(GLOB_RECURSE PLATFORM_SRC
    src/_platform/*.cpp
)

add_executable(gamedaddy
    src/main.cpp
    ${PLATFORM_SRC}
)

# SDL only belongs to executable, NOT tests
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

target_link_libraries(gamedaddy PRIVATE
    gamedaddy_core
    SDL2::SDL2
    SDL2_image::SDL2_image
)

# ---------------------------
# Tests (private with gtest)
# ---------------------------
enable_testing()

file(GLOB TEST_SOURCES test/*.cpp)

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE
    gamedaddy_core
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
