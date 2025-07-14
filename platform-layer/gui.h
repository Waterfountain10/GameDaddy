//
// Created by William Lafond on 2025-07-13.
//

#ifndef GUI_H
#define GUI_H

#include <memory>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>

namespace GameBoy {

    class Gui {
    public:
        Gui(const std::string name, int width, int height, bool is_demo); // constructor
        ~Gui();
        void clear();
        void present_idle();
        bool input_rom_path(std::string rom_path);
        void draw_pixel(int col, int row /* int scale */, bool on); // on, paint white, off is nothing

        int update_texture(uint8_t* gfx_ptr);

    private:
        SDL_Window* win = nullptr;
        SDL_Texture*  screen_texture;
        std::unique_ptr<SDL_Rect> screen_rect;
        SDL_Renderer* ren = nullptr;
        SDL_Color* colors;

        uint8_t width;
        uint8_t height;

        bool intro;

        SDL_Color* init_colors();
    };

}

#endif //gui_h
