#include "network_edge.hpp"

 void DrawXEdge::draw()  {
    fl_color(FL_BLACK);
    switch (direction) {
    case _TOP_LEFT:
        fl_line(x(), y(), x() + w(), y() + h());
        break;
    case _BOT_LEFT:
        fl_line(x(), y() + h(), x() + w(), y());
        break;
    }
}
