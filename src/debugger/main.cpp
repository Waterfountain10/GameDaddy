#include <SDL.h>
#include <array>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;
};

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

    void step_instruction() {
        if (current_instruction + 1 < static_cast<int>(instructions.size()))
            ++current_instruction;
        cycles += 4;
    }

    void step_frame() {
        ++frames;
        cycles += 70224;
    }

    void toggle_run() {
        paused = !paused;
    }
};

using Glyph = std::array<std::string_view, 7>;

const Glyph& glyph_for(char raw) {
    static const Glyph blank{"     ", "     ", "     ", "     ", "     ", "     ", "     "};
    static const std::unordered_map<char, Glyph> glyphs{
        {' ', blank},
        {'!', {"  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "     ", "  X  "}},
        {'-', {"     ", "     ", "     ", " XXX ", "     ", "     ", "     "}},
        {'_', {"     ", "     ", "     ", "     ", "     ", "     ", "XXXXX"}},
        {'.', {"     ", "     ", "     ", "     ", "     ", " XX  ", " XX  "}},
        {':', {"     ", " XX  ", " XX  ", "     ", " XX  ", " XX  ", "     "}},
        {'/', {"    X", "   X ", "   X ", "  X  ", " X   ", " X   ", "X    "}},
        {'>', {"X    ", " X   ", "  X  ", "   X ", "  X  ", " X   ", "X    "}},
        {'0', {" XXX ", "X   X", "X  XX", "X X X", "XX  X", "X   X", " XXX "}},
        {'1', {"  X  ", " XX  ", "X X  ", "  X  ", "  X  ", "  X  ", "XXXXX"}},
        {'2', {" XXX ", "X   X", "    X", "   X ", "  X  ", " X   ", "XXXXX"}},
        {'3', {" XXX ", "X   X", "    X", "  XX ", "    X", "X   X", " XXX "}},
        {'4', {"   X ", "  XX ", " X X ", "X  X ", "XXXXX", "   X ", "   X "}},
        {'5', {"XXXXX", "X    ", "XXXX ", "    X", "    X", "X   X", " XXX "}},
        {'6', {" XXX ", "X   X", "X    ", "XXXX ", "X   X", "X   X", " XXX "}},
        {'7', {"XXXXX", "    X", "   X ", "  X  ", " X   ", " X   ", " X   "}},
        {'8', {" XXX ", "X   X", "X   X", " XXX ", "X   X", "X   X", " XXX "}},
        {'9', {" XXX ", "X   X", "X   X", " XXXX", "    X", "X   X", " XXX "}},
        {'A', {" XXX ", "X   X", "X   X", "XXXXX", "X   X", "X   X", "X   X"}},
        {'B', {"XXXX ", "X   X", "X   X", "XXXX ", "X   X", "X   X", "XXXX "}},
        {'C', {" XXX ", "X   X", "X    ", "X    ", "X    ", "X   X", " XXX "}},
        {'D', {"XXXX ", "X   X", "X   X", "X   X", "X   X", "X   X", "XXXX "}},
        {'E', {"XXXXX", "X    ", "X    ", "XXXX ", "X    ", "X    ", "XXXXX"}},
        {'F', {"XXXXX", "X    ", "X    ", "XXXX ", "X    ", "X    ", "X    "}},
        {'G', {" XXX ", "X   X", "X    ", "X XXX", "X   X", "X   X", " XXX "}},
        {'H', {"X   X", "X   X", "X   X", "XXXXX", "X   X", "X   X", "X   X"}},
        {'I', {"XXXXX", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "XXXXX"}},
        {'J', {"XXXXX", "    X", "    X", "    X", "    X", "X   X", " XXX "}},
        {'K', {"X   X", "X  X ", "X X  ", "XX   ", "X X  ", "X  X ", "X   X"}},
        {'L', {"X    ", "X    ", "X    ", "X    ", "X    ", "X    ", "XXXXX"}},
        {'M', {"X   X", "XX XX", "X X X", "X   X", "X   X", "X   X", "X   X"}},
        {'N', {"X   X", "XX  X", "XX  X", "X X X", "X  XX", "X  XX", "X   X"}},
        {'O', {" XXX ", "X   X", "X   X", "X   X", "X   X", "X   X", " XXX "}},
        {'P', {"XXXX ", "X   X", "X   X", "XXXX ", "X    ", "X    ", "X    "}},
        {'Q', {" XXX ", "X   X", "X   X", "X   X", "X X X", "X  X ", " XX X"}},
        {'R', {"XXXX ", "X   X", "X   X", "XXXX ", "X X  ", "X  X ", "X   X"}},
        {'S', {" XXXX", "X    ", "X    ", " XXX ", "    X", "    X", "XXXX "}},
        {'T', {"XXXXX", "  X  ", "  X  ", "  X  ", "  X  ", "  X  ", "  X  "}},
        {'U', {"X   X", "X   X", "X   X", "X   X", "X   X", "X   X", " XXX "}},
        {'V', {"X   X", "X   X", "X   X", "X   X", "X   X", " X X ", "  X  "}},
        {'W', {"X   X", "X   X", "X   X", "X   X", "X X X", "XX XX", "X   X"}},
        {'X', {"X   X", "X   X", " X X ", "  X  ", " X X ", "X   X", "X   X"}},
        {'Y', {"X   X", "X   X", " X X ", "  X  ", "  X  ", "  X  ", "  X  "}},
        {'Z', {"XXXXX", "    X", "   X ", "  X  ", " X   ", "X    ", "XXXXX"}},
    };

    const char c  = static_cast<char>(std::toupper(static_cast<unsigned char>(raw)));
    const auto it = glyphs.find(c);
    return it == glyphs.end() ? blank : it->second;
}

void set_color(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void draw_text(SDL_Renderer* renderer, int x, int y, std::string_view text, Color color,
               int scale = 2) {
    set_color(renderer, color);
    int cursor_x = x;
    for (char c : text) {
        const auto& glyph = glyph_for(c);
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (glyph[row][col] == 'X') {
                    SDL_Rect pixel{cursor_x + col * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursor_x += 6 * scale;
    }
}

void draw_panel(SDL_Renderer* renderer, const Panel& panel) {
    set_color(renderer, {24, 27, 31});
    SDL_RenderFillRect(renderer, &panel.rect);
    set_color(renderer, {90, 105, 120});
    SDL_RenderDrawRect(renderer, &panel.rect);
    draw_text(renderer, panel.rect.x + 10, panel.rect.y + 8, panel.title, {164, 190, 212}, 2);
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string rom_path = argc > 1 ? argv[1] : "roms/pokemon-red.gb";

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Error initializing SDL debugger: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("GameDaddy Debugger", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1180, 720, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "GameDaddy debugger window could not be opened: " << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    DebuggerState state;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
                running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
                state.step_instruction();
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f)
                state.step_frame();
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
                state.toggle_run();
        }

        if (!state.paused && state.frames % 8 == 0)
            state.step_instruction();
        ++state.frames;

        set_color(renderer, {12, 14, 16});
        SDL_RenderClear(renderer);

        const Panel screen{"SCREEN", {24, 24, 640, 576}};
        const Panel cpu{"CPU", {684, 24, 220, 150}};
        const Panel controls{"CONTROLS", {924, 24, 232, 150}};
        const Panel instructions{"INSTRUCTIONS", {684, 194, 472, 246}};
        const Panel memory{"MEMORY MAP", {24, 620, 640, 76}};
        const Panel profiler{"PROFILER", {684, 460, 472, 236}};

        draw_panel(renderer, screen);
        draw_panel(renderer, cpu);
        draw_panel(renderer, controls);
        draw_panel(renderer, instructions);
        draw_panel(renderer, memory);
        draw_panel(renderer, profiler);

        SDL_Rect viewport{104, 96, 480, 432};
        set_color(renderer, {167, 183, 78});
        SDL_RenderFillRect(renderer, &viewport);
        set_color(renderer, {40, 48, 22});
        SDL_RenderDrawRect(renderer, &viewport);
        draw_text(renderer, 216, 300, "GAME BOY VIEWPORT", {40, 48, 22}, 2);

        draw_text(renderer, 704, 58, "A 01 F B0", {215, 223, 232}, 2);
        draw_text(renderer, 704, 82, "B 00 C 13", {215, 223, 232}, 2);
        draw_text(renderer, 704, 106, "PC 0100", {215, 223, 232}, 2);
        draw_text(renderer, 704, 130, "SP FFFE", {215, 223, 232}, 2);

        draw_text(renderer, 944, 58, "Q QUIT", {215, 223, 232}, 2);
        draw_text(renderer, 944, 82, "S STEP", {215, 223, 232}, 2);
        draw_text(renderer, 944, 106, "F FRAME", {215, 223, 232}, 2);
        draw_text(renderer, 944, 130, state.paused ? "R RUN" : "R PAUSE", {215, 223, 232}, 2);

        int trace_y = 230;
        for (int i = 0; i < static_cast<int>(state.instructions.size()); ++i) {
            const bool current = i == state.current_instruction;
            draw_text(renderer, 704, trace_y, current ? ">>>" : "   ",
                      current ? Color{255, 205, 92} : Color{116, 132, 148}, 2);
            draw_text(renderer, 746, trace_y, state.instructions[i],
                      current ? Color{255, 205, 92} : Color{215, 223, 232}, 2);
            trace_y += 24;
        }

        draw_text(renderer, 44, 654, "ROM VRAM CART RAM WRAM OAM IO HRAM IE", {215, 223, 232}, 2);
        draw_text(renderer, 704, 494, std::string("MODE ") + (state.paused ? "PAUSED" : "RUNNING"),
                  {215, 223, 232}, 2);
        draw_text(renderer, 704, 522, std::string("FRAMES ") + std::to_string(state.frames),
                  {215, 223, 232}, 2);
        draw_text(renderer, 704, 550, std::string("CYCLES ") + std::to_string(state.cycles),
                  {215, 223, 232}, 2);
        draw_text(renderer, 704, 578, "FPS TARGET 60", {215, 223, 232}, 2);
        draw_text(renderer, 704, 606, "HOT OPC 00 NOP", {215, 223, 232}, 2);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Closed GameDaddy debugger for " << rom_path << std::endl;
    return 0;
}
