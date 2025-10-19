#pragma once
// The NXP Network Project -- network_node_group
#include <FL/Fl_Group.H>

#include "network_node_struct.h"
#include "network_edge.hpp"


class DrawXNodeGroup : public Fl_Group {
    Node* node;
    Fl_Widget* pw = nullptr;
    DrawXEdge* pedge=nullptr;

public:
    DrawXNodeGroup(Node* n, int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(_COLORBG);
        node = n;
    }

    virtual void draw() FL_OVERRIDE;

    Node* get_node();
    
    void set_pw(Fl_Widget* parent_wgt);
    Fl_Widget* get_pw();
    
    void set_pedge(DrawXEdge* parent_edge);
    DrawXEdge* get_pedge();

};
