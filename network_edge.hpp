#pragma once
// The NXP Network Project -- network_edge
#include <FL/Fl.H>
#include <FL/fl_draw.H>

const uint8_t  _TOP_LEFT = 1;
const uint8_t  _BOT_LEFT = 2;

class DrawXEdge : public Fl_Widget {
    uint8_t direction;

public:
    DrawXEdge(uint8_t dir, int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
        direction = dir;
    }

    virtual void draw() FL_OVERRIDE;

};