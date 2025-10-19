#pragma once
#include <array>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>

#include "network_node_struct.h"
#include "network_node.hpp"


class DrawX : public Fl_Group {
    Fl_Scroll* _scroll = nullptr;
    Node* root = nullptr;
    Network* net;
    double xmax = 0;
    double ymax = 0;

    void edge_attach(DrawXNodeGroup* wgt, double* x2ptr, double* y2ptr);
    void edge_attach(DrawXNode* wgt, double* x2ptr, double* y2ptr);
    void get_max_xy(Node* node);
    void add_children_nodes(Node* node, DrawXNodeGroup* parent_wgt);
    void add_children_nodes(Node* node, DrawXNode* parent_wgt);

public:

    DrawX(Fl_Scroll* sc, Node* r, Network* netw, int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
        root = r;
        net = netw;
        _scroll = sc;
        // reset();
    }

    virtual void draw() FL_OVERRIDE;

    void reset();

    Node* get_root();
    void  set_root( Node* r);

    Fl_Scroll* scroll();
    void scroll(Fl_Scroll* sc);
};

