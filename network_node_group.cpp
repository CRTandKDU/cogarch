#include "network_node_group.hpp"

 void DrawXNodeGroup::draw() {
    fl_color(color()); fl_rectf(x(), y(), w(), h());
    draw_children();
    //
    int x0 = x();
    int y0 = y();
    for (short i = 1; i < node->groups.size(); i++) {
        y0 += _W;
        fl_color(color());
        fl_rectf(x0 + 1, y0 - 1, w() - 2, 2);
        fl_color(FL_GRAY);
        fl_line(x0, y0, x0 + w(), y0);
    }
}

Node* DrawXNodeGroup::get_node() { return node; }

void DrawXNodeGroup::set_pw(DrawXNodeGroup* parent_wgt) { pw = parent_wgt; }
DrawXNodeGroup* DrawXNodeGroup::get_pw() { return pw; }

void DrawXNodeGroup::set_pedge(DrawXEdge* parent_edge) { pedge = parent_edge; }
DrawXEdge* DrawXNodeGroup::get_pedge() { return pedge; }

