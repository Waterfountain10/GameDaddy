#!/usr/bin/env bash
set -e

cmake --preset debug
cmake --build --preset debug -j

lldb ./build/debug/gamedaddy -- roms/pokemon-red.gb
