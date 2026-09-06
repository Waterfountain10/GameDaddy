#pragma once

#include <SDL_render.h>
#include <cstdint>
#include <string_view>

namespace GameBoy::Debugger {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;
};

void set_color(SDL_Renderer* renderer, Color color);
void draw_text(SDL_Renderer* renderer, int x, int y, std::string_view text, Color color,
               int scale = 2);

} // namespace GameBoy::Debugger
