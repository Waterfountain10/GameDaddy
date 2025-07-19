//
// Created by William Lafond on 2025-07-17.
//

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

class DisplayInterface {
public:
    virtual ~DisplayInterface() = default; // uses compiler default deconstructor
    virtual void clear() = 0;
    virtual void present_idle() = 0;
    virtual void draw_pixel(int col, int row, bool draw_on);
    int update_texture(int* gfx_ptr);
protected:
    int display_width;
    int display_height;
// base classes or interfaces should not have private methods/fields
};

#endif //DISPLAY_INTERFACE_H