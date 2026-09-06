#pragma once

#include "debug_text.h"

#include <SDL_rect.h>
#include <SDL_render.h>
#include <string>
#include <vector>

namespace GameBoy::Debugger {

struct Panel {
    std::string title;
    SDL_Rect    rect;
};

struct DebuggerState {
    bool                     paused              = true;
    int                      current_instruction = 3;
    uint64_t                 frames              = 0;
    uint64_t                 cycles              = 0;
    std::vector<std::string> instructions{"00FA LD A 01", "00FC LDH FF50 A", "00FE JP 0100",
                                          "0100 NOP",     "0101 LD BC D16",  "0104 INC B",
                                          "0105 DEC B",   "0106 JR NZ R8"};

    void step_instruction();
    void step_frame();
    void toggle_run();
};

class VisualDebugger {
public:
    explicit VisualDebugger(std::string rom_path);
    int run();

private:
    void handle_events(bool& running);
    void update();
    void render();
    void draw_panel(const Panel& panel);

    std::string   rom_path_;
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    DebuggerState state_;
};

} // namespace GameBoy::Debugger
