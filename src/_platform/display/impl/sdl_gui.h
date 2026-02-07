//
// Created by William Lafond on 2025-07-13.
//

#ifndef GUI_H
#define GUI_H

#include <memory>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>

#include "../display_interface.h"

namespace GameBoy {

class SDLGui : public DisplayInterface {
public:
    SDLGui(int w, int h);
    ~SDLGui() override;
    void clear() override;
    void present_idle() override;
    void draw_pixel(int col, int row, bool draw_on, int color_index) override;
    int update_screen(int* gfx_ptr) override;
private:
    SDL_Color* init_colors();
    SDL_Window* win = nullptr;
    SDL_Texture*  screen_texture;
    std::unique_ptr<SDL_Rect> screen_rect;
    SDL_Renderer* ren = nullptr;
    SDL_Color* colors;
    // add private helpers for gui related stuff if needed.
};
}

#endif //SDL_GUI_H
