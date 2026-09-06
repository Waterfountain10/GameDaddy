#include "debug_text.h"

#include <array>
#include <cctype>
#include <string_view>
#include <unordered_map>

namespace GameBoy::Debugger {
namespace {

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

} // namespace

void set_color(SDL_Renderer* renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void draw_text(SDL_Renderer* renderer, int x, int y, std::string_view text, Color color,
               int scale) {
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

} // namespace GameBoy::Debugger
