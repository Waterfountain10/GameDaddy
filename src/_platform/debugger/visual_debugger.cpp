#include "visual_debugger.h"

#include <SDL.h>
#include <iostream>

namespace GameBoy::Debugger {

void DebuggerState::step_instruction() {
    if (current_instruction + 1 < static_cast<int>(instructions.size()))
        ++current_instruction;
    cycles += 4;
}

void DebuggerState::step_frame() {
    ++frames;
    cycles += 70224;
}

void DebuggerState::toggle_run() {
    paused = !paused;
}

VisualDebugger::VisualDebugger(std::string rom_path) : rom_path_{std::move(rom_path)} {}

int VisualDebugger::run() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Error initializing SDL debugger: " << SDL_GetError() << std::endl;
        return 1;
    }

    window_ = SDL_CreateWindow("GameDaddy Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               1180, 720, SDL_WINDOW_RESIZABLE);
    if (!window_) {
        std::cerr << "GameDaddy debugger window could not be opened: " << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        handle_events(running);
        update();
        render();
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();

    std::cout << "Closed GameDaddy debugger for " << rom_path_ << std::endl;
    return 0;
}

void VisualDebugger::handle_events(bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
            running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
            state_.step_instruction();
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f)
            state_.step_frame();
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
            state_.toggle_run();
    }
}

void VisualDebugger::update() {
    if (!state_.paused && state_.frames % 8 == 0)
        state_.step_instruction();
    ++state_.frames;
}

void VisualDebugger::draw_panel(const Panel& panel) {
    set_color(renderer_, {24, 27, 31});
    SDL_RenderFillRect(renderer_, &panel.rect);
    set_color(renderer_, {90, 105, 120});
    SDL_RenderDrawRect(renderer_, &panel.rect);
    draw_text(renderer_, panel.rect.x + 10, panel.rect.y + 8, panel.title, {164, 190, 212}, 2);
}

void VisualDebugger::render() {
    set_color(renderer_, {12, 14, 16});
    SDL_RenderClear(renderer_);

    const Panel screen{"SCREEN", {24, 24, 640, 576}};
    const Panel cpu{"CPU", {684, 24, 220, 150}};
    const Panel controls{"CONTROLS", {924, 24, 232, 150}};
    const Panel instructions{"INSTRUCTIONS", {684, 194, 472, 246}};
    const Panel memory{"MEMORY MAP", {24, 620, 640, 76}};
    const Panel profiler{"PROFILER", {684, 460, 472, 236}};

    draw_panel(screen);
    draw_panel(cpu);
    draw_panel(controls);
    draw_panel(instructions);
    draw_panel(memory);
    draw_panel(profiler);

    SDL_Rect viewport{104, 96, 480, 432};
    set_color(renderer_, {167, 183, 78});
    SDL_RenderFillRect(renderer_, &viewport);
    set_color(renderer_, {40, 48, 22});
    SDL_RenderDrawRect(renderer_, &viewport);
    draw_text(renderer_, 216, 300, "GAME BOY VIEWPORT", {40, 48, 22}, 2);

    draw_text(renderer_, 704, 58, "A 01 F B0", {215, 223, 232}, 2);
    draw_text(renderer_, 704, 82, "B 00 C 13", {215, 223, 232}, 2);
    draw_text(renderer_, 704, 106, "PC 0100", {215, 223, 232}, 2);
    draw_text(renderer_, 704, 130, "SP FFFE", {215, 223, 232}, 2);

    draw_text(renderer_, 944, 58, "Q QUIT", {215, 223, 232}, 2);
    draw_text(renderer_, 944, 82, "S STEP", {215, 223, 232}, 2);
    draw_text(renderer_, 944, 106, "F FRAME", {215, 223, 232}, 2);
    draw_text(renderer_, 944, 130, state_.paused ? "R RUN" : "R PAUSE", {215, 223, 232}, 2);

    int trace_y = 230;
    for (int i = 0; i < static_cast<int>(state_.instructions.size()); ++i) {
        const bool current = i == state_.current_instruction;
        draw_text(renderer_, 704, trace_y, current ? ">>>" : "   ",
                  current ? Color{255, 205, 92} : Color{116, 132, 148}, 2);
        draw_text(renderer_, 746, trace_y, state_.instructions[i],
                  current ? Color{255, 205, 92} : Color{215, 223, 232}, 2);
        trace_y += 24;
    }

    draw_text(renderer_, 44, 654, "ROM VRAM CART RAM WRAM OAM IO HRAM IE", {215, 223, 232}, 2);
    draw_text(renderer_, 704, 494, std::string("MODE ") + (state_.paused ? "PAUSED" : "RUNNING"),
              {215, 223, 232}, 2);
    draw_text(renderer_, 704, 522, std::string("FRAMES ") + std::to_string(state_.frames),
              {215, 223, 232}, 2);
    draw_text(renderer_, 704, 550, std::string("CYCLES ") + std::to_string(state_.cycles),
              {215, 223, 232}, 2);
    draw_text(renderer_, 704, 578, "FPS TARGET 60", {215, 223, 232}, 2);
    draw_text(renderer_, 704, 606, "HOT OPC 00 NOP", {215, 223, 232}, 2);

    SDL_RenderPresent(renderer_);
}

} // namespace GameBoy::Debugger
