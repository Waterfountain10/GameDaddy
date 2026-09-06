#include "visual_debugger.h"

#include <string>

int main(int argc, char* argv[]) {
    const std::string                 rom_path = argc > 1 ? argv[1] : "roms/pokemon-red.gb";
    GameBoy::Debugger::VisualDebugger debugger{rom_path};
    return debugger.run();
}
