//
// Created by William Lafond on 2025-07-17.
//

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

namespace GameBoy {
class DisplayInterface {
public:
    virtual ~DisplayInterface() = default; // uses compiler default deconstructor
    virtual void clear() = 0;
    virtual void present_idle() = 0;
    virtual void draw_pixel(int col, int row, bool draw_on, int color_index) = 0;
    virtual int update_screen(int* gfx_buffer_ptr) = 0;
protected:
    int display_width = 0;
    int display_height = 0;
    // base classes or interfaces should not have private methods/fields
};
}

#endif //DISPLAY_INTERFACE_H