//
// Created by William Lafond on 2025-07-13.
//

#include "sdl_gui.h"

#include <iostream>

#include <SDL_events.h>
#include <SDL_video.h>
#include <SDL.h>

namespace GameBoy {
    /**
     * @brief Constructs the GUI with a window, renderer, and palette.
     *
     * Initializes SDL2 window and renderer, sets logical size to 64x32 for GameBoy,
     * configures the background color based on intro flag, creates a paletted surface,
     * and prepares the initial render.
     *
     * @param name Title of the SDL window.
     * @param w Width of the window in pixels.
     * @param h Height of the window in pixels.
     * @param is_intro If true, sets up the intro selection screen background color.
     * @throws std::runtime_error if the SDL window or renderer cannot be created.
     */
    SDLGui::SDLGui(int w, int h) {
        // set protected display resolutions
        display_width = w;
        display_height = h;

        // initialize a window display
        this->win = SDL_CreateWindow("GameDaddy",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, w, h,
                                     SDL_WINDOW_RESIZABLE);
        if (!win)
            throw std::runtime_error("GameDaddy's GUI could not be opened!");
        this->ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        if (!ren) throw std::runtime_error("SDL_CreateRenderer failed");
        SDL_RenderSetLogicalSize(this->ren, 64, 32); // fixed 64, 32 to allow responsive scaling
        SDL_SetRenderDrawColor(ren, 10, 10, 10, 255);
        SDL_Surface* screen_surface = SDL_CreateRGBSurfaceWithFormat(0,w,h,1,
                SDL_PIXELFORMAT_INDEX8);

        //  handle exceptions
        if (!screen_surface) {
            SDL_Log("Surface has no palette?!\n");
            SDL_FreeSurface(screen_surface);
            return;
        }
        SDL_Palette* palette = screen_surface->format->palette;
        if (!palette) {
            SDL_Log("Surface has no palette?!\n");
            SDL_FreeSurface(screen_surface);
            return;
        }
        SDL_SetPaletteColors(palette, init_colors(), 0, 4);
        screen_texture = SDL_CreateTextureFromSurface(ren, screen_surface);
        screen_rect = std::make_unique<SDL_Rect>( SDL_Rect{ 0, 0, w, h } );

        SDL_FreeSurface(screen_surface);
        // one time clear + present so you see something right away
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, screen_texture, NULL, screen_rect.get());
        SDL_RenderPresent(ren);
    }

    /**
     * @brief Initializes the color palette for the display (black and white).
     *
     * Allocates an array of two SDL_Color values and sets index 0 to black
     * and index 1 to white for GameBoy's monochrome display.
     *
     * @return Pointer to the dynamically allocated SDL_Color array.
     */
    SDL_Color* SDLGui::init_colors() {
        colors = static_cast<SDL_Color*>(malloc(sizeof(SDL_Color) * 4));

        // DMG (aka original GameBoy) 4-color palette wheel
        colors[0] = SDL_Color{255, 255, 255, 255}; // White
        colors[1] = SDL_Color{192, 192, 192, 255}; // Light Gray
        colors[2] = SDL_Color{96,  96,  96,  255}; // Dark Gray
        colors[3] = SDL_Color{0,   0,   0,   255}; // Black

        return colors;
    }

    /**
     * @brief Destructor for the GUI, cleans up SDL resources and allocated memory.
     *
     * Destroys the SDL texture, renderer, and window, and frees the palette colors.
     */
    SDLGui::~SDLGui() {
        if (screen_texture) SDL_DestroyTexture(screen_texture);
        if (ren) SDL_DestroyRenderer(ren);
        if (win) SDL_DestroyWindow(win);
        if (colors) free(colors);
        ren = nullptr; win = nullptr; screen_texture = nullptr; screen_rect = nullptr;
    }

    /**
     * @brief Clears the renderer with a default background color.
     *
     * Sets the draw color to a dark grey and clears the current rendering target.
     */
    void SDLGui::clear() {
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);
    }

    /**
     * @brief Presents the current rendered frame to the display without clearing.
     *
     * Useful for idle or static frames where only presenting is needed.
     */
    void SDLGui::present_idle() {
        SDL_RenderPresent(ren);
    }

    /**
     * Draws a single GameBoy “pixel” as a filled rectangle on the screen.
     *
     *
     * @param col    The horizontal coordinate of the pixel (0–63).
     * @param row    The vertical coordinate of the pixel (0–31).
     * @param on     If true, draw the pixel (white); if false, do nothing (pixel remains off).
     * @param color_index Index 0-3 for the dmg color palette
    */
    void SDLGui::draw_pixel(int col, int row, bool on, int color_index)
    {
        if (!on) return;
        SDL_Color pixel_color = colors[color_index];
        SDL_SetRenderDrawColor(ren, pixel_color.r, pixel_color.g, pixel_color.b, pixel_color.a);
        SDL_Rect r{col, row, 1, 1}; // draws a 1x1 logical pixel in canvas always
        SDL_RenderFillRect(ren, &r);
    }

    /**
     * @brief Updates the screen texture from the graphics buffer and renders it.
     *
     * Copies the provided 8-bit graphics buffer into the SDL texture, then
     * renders the texture to the window.
     *
     * @param gfx_ptr Pointer to the graphics buffer (uint8_t array) sized width * height.
     * @return The result code from SDL_RenderCopy (0 on success, negative on failure).
     */
    int SDLGui::update_screen(int* gfx_ptr) {
        SDL_UpdateTexture(screen_texture, nullptr, gfx_ptr, display_width * sizeof(int));
        return SDL_RenderCopy(ren, screen_texture, NULL, screen_rect.get());
    }

}
