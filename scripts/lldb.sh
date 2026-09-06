#!/usr/bin/env bash
set -e

cmake --preset debug
cmake --build --preset debug -j

lldb ./build/debug/gamedaddy -- "${1:-roms/pokemon-red.gb}"
