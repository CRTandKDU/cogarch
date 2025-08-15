#pragma once
#include <array>

#include <FL/Fl_Group.H>

#include "network_node_struct.h"

class DrawX : public Fl_Group {
    Node* root = nullptr;
    double xmax = 0;
    double ymax = 0;

    void edge_attach(DrawXNodeGroup* wgt, double* x2ptr, double* y2ptr);

    void get_max_xy(Node* node);

    void add_children_nodes(Node* node, DrawXNodeGroup* parent_wgt);

public:

    DrawX(Node* r, int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
        root = r;
        reset();
    }

    virtual void draw() FL_OVERRIDE;

    void reset();

    Node* get_root();

};

