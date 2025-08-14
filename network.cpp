#include <cstdint>
#include <cstdarg>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <format>
#include <array>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>

#include "layout.hpp"

constexpr auto _W = 20.;
constexpr auto _H = 100.;
constexpr auto _LMARGIN = 2;
constexpr auto _TMARGIN = 1;

const uint32_t _COLORBG          = 0xFFFFFF00;
const uint32_t _COLORFG          = 0x00000000;
const uint32_t _SELECTION_COLOR  = 0x9575CD00;
const uint32_t _HOVER_COLORBG    = 0x18549E00;
const uint32_t _HOVER_COLORFG    = 0xFFFFFF00;

const uint8_t  _TOP_LEFT = 1;
const uint8_t  _BOT_LEFT = 2;

//------------------------------------------------------------------------------------------------------
struct Node {
    double x, y, w, h;
    double prelim = 0, mod = 0, shift = 0, change = 0;
    Node* parent = nullptr;
    std::vector<Node*> children;
    Node* tl = nullptr, * tr = nullptr, * el = nullptr, * er = nullptr;
    double msel = 0, mser = 0;
    // Clientdata
    std::string text = "";
    std::vector< std::vector<Node*> > groups = {};
};

void render_str(Node* top, int level) {
    std::string s = std::string("").append(level, ' ');
    std::cout << s.append(std::format("{}: x = {}, y = {}", top->text, top->x, top->y)) << std::endl;
    for (Node* child : top->children) {
        render_str(child, level + 2);
    }
}

void build_node_plain(Node* top, std::string topname, int count, ...) {
    va_list args;
    va_start(args, count);
    //
    top->w = _W; top->h = _H; top->text = std::string(topname);
    for (short i = 0; i < count; i++) {
        Node* child = va_arg(args, Node*);
        top->children.push_back(child);
        child->parent = top;
    }
    va_end(args);
}

void build_node_group(Node* top, std::string topname, int count, ...) {
    va_list args;
    va_start(args, count);
    //
    top->w = _W * count; top->h = _H; top->text = std::string(topname);

    for (short i = 0; i < count; i++) {
        std::vector<Node*> group = va_arg(args, std::vector<Node*>);
        top->groups.insert(top->groups.begin(), group);
        for (Node* child : group) {
            top->children.push_back(child);
            child->parent = top;
        }
    }
    va_end(args);
}

//------------------------------------------------------------------------------------------------------

class DrawXEdge : public Fl_Widget {
    uint8_t direction;

public:
    DrawXEdge(uint8_t dir, int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
        direction = dir;
    }

    virtual void draw() FL_OVERRIDE {
        fl_color(FL_BLACK);
        switch (direction) {
        case _TOP_LEFT:
            fl_line(x(), y(), x() + w(), y() + h());
            break;
        case _BOT_LEFT:
            fl_line(x(), y()+h(), x() + w(), y() );
            break;
        }
    }

};


class DrawXNodeGroup : public Fl_Group {
    Node* node;
    DrawXNodeGroup* pw;
    DrawXEdge* pedge;

public:
    DrawXNodeGroup(Node* n, int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(_COLORBG);
        node = n;
    }

    virtual void draw() FL_OVERRIDE {
        fl_color(color()); fl_rectf(x(), y(), w(), h());
        draw_children();
        //
        int x0 = x();
        int y0 = y();
        for (short i = 1; i < node->groups.size(); i++) {
            y0 += _W;
            fl_color(color());
            fl_rectf(x0+1, y0-1, w()-2, 2);
            fl_color(FL_GRAY);
            fl_line(x0, y0, x0 + w(), y0);
        }
    }

    Node* get_node() { return node; }
    void set_pw(DrawXNodeGroup* parent_wgt) { pw = parent_wgt; }
    DrawXNodeGroup* get_pw() { return pw; }
    void set_pedge(DrawXEdge* parent_edge) { pedge = parent_edge; }
    DrawXEdge* get_pedge() { return pedge; }

};

class DrawXNode : public Fl_Widget {
    Node* node;
    DrawXNodeGroup* pw;
    DrawXEdge* pedge;
    Fl_Color style_fg_color;
    Fl_Font style_font;
    const int style_font_size = _W - _TMARGIN - _TMARGIN;

public:
    DrawXNode(Node* n, int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(_COLORBG);
        node = n;
        style_fg_color = _COLORFG;
        style_font = FL_COURIER;
    }

    virtual void draw() FL_OVERRIDE {
        // Draw background - a white filled rectangle
        fl_color(color()); fl_rectf(x(), y(), w(), h());
        // Draw node
        if (node) {
            fl_color(FL_BLACK);
            fl_rect(x(), y(), w(), h());
            fl_color(style_fg_color);
            fl_font(style_font, style_font_size);
            fl_draw(node->text.c_str(), _LMARGIN + x(), y(), w(), h(),
                FL_ALIGN_TOP_LEFT, nullptr, 1, 0);
        }
    }

    int handle(int event) {
        int ret = Fl_Widget::handle(event);
        switch (event) {
        case FL_ENTER:
            color(_HOVER_COLORBG);
            set_fg_color(_HOVER_COLORFG);
            redraw();
            return 1;
            break;
        case FL_LEAVE:
            color(_COLORBG);
            set_fg_color(_COLORFG);
            redraw();
            return 1;
            break;
        }
        return ret;
    }

    void set_pw(DrawXNodeGroup* parent_wgt) { pw = parent_wgt; }
    DrawXNodeGroup* get_pw() { return pw; }
    void set_pedge(DrawXEdge* parent_edge) { pedge = parent_edge; }
    DrawXEdge* get_pedge() { return pedge; }

    Node* get_node() { return node; }

    void set_fg_color(Fl_Color fgc) { style_fg_color = fgc; }
};


class DrawX : public Fl_Group {
    Node* root = nullptr;
    int xmax = 0;

    void edge_attach(DrawXNodeGroup *wgt, double* x2ptr, double* y2ptr) {
        DrawXNodeGroup* pw = wgt->get_pw();
        DrawXNode* swgt = nullptr;

        *x2ptr = pw->x() + pw->w();
        //
        if (1 > pw->get_node()->groups.size()) {
            *y2ptr = pw->y() + pw->h() / 2.;
        }
        else {
            std::vector< std::vector<Node*> > gvec = pw->get_node()->groups;
            for (short i = 0; i < gvec.size(); i++) {
                if (std::find(gvec[i].begin(), gvec[i].end(), wgt->get_node()) != gvec[i].end()) {
                    swgt = (DrawXNode * ) pw->child(i);
                    *y2ptr = swgt->y() + swgt->h() / 2;
                    return;
                }
            }
            // Not found!
            *y2ptr = pw->y() + pw->h() / 2.;
        }
    }

    void get_max_x(Node* node) {
        if (node->x + node->w > xmax) xmax = node->x + node->w;
        for (Node* child : node->children) {
            get_max_x(child);
        }
    }

    void add_children_nodes(Node* node, DrawXNodeGroup *parent_wgt) {
        double xnew, ynew, wnew, hnew;
        short i;
        const int font_size = _W - _TMARGIN - _TMARGIN;
        // Transform coord.
        xnew = node->y; ynew = xmax - node->x - node->w;
        wnew = node->h; hnew = node->w;
        //
        DrawXNodeGroup* wgt = nullptr;
        DrawXNode *swgt = nullptr;
        if (node->groups.size() > 0) {
            // Node is a compact
			wgt = new DrawXNodeGroup(node, x() + xnew, y() + ynew, wnew, hnew);
			for (i = 0; i < node->groups.size(); i++) {
				swgt = new DrawXNode(node, x() + xnew, y() + ynew + i * _W, wnew, _W);
				swgt->set_pw(wgt);
				wgt->add(swgt);
			}
            wgt->set_pw(parent_wgt);
        }
        else {
            // Leaf represented as a single compact
            wgt = new DrawXNodeGroup(node, x() + xnew, y() + ynew, wnew, hnew);
            swgt = new DrawXNode(node, x() + xnew, y() + ynew , wnew, hnew);
            swgt->set_pw(wgt);
            wgt->add(swgt);
            wgt->set_pw(parent_wgt);
        }
        
        add(wgt);

        if (parent_wgt) {
            DrawXEdge* edge;
            double x1 = wgt->x();
            double y1 = wgt->y() + wgt->h() / 2.;
            double x2 ;
            double y2 ;
            edge_attach(wgt, &x2, &y2);
            if (y1 < y2) {
                edge = new DrawXEdge(_BOT_LEFT, x2, y1, x1 - x2, y2 - y1);
            }
            else {
                edge = new DrawXEdge(_TOP_LEFT, x2, y2, x1 - x2, y1 - y2);
            }
            add(edge);
            wgt->set_pedge(edge);
        }
        for (Node* child : node->children) {
            add_children_nodes(child, wgt);
        }
    }



public:

    DrawX(Node *r, int X, int Y, int W, int H, const char* L = 0) : Fl_Group(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
        root = r;
        xmax = 0;
        get_max_x(root);
        // Build depth-first traversal list of children node-widgets
        add_children_nodes(root, nullptr);
        std::cout << "Xmax: " << xmax << std::endl;
    }
    
    virtual void draw() FL_OVERRIDE {
        std::array<double, 2> root_pt;
        // Draw background - a white filled rectangle
        fl_color(color()); fl_rectf(x(), y(), w(), h());
        //
        draw_children();
    }

};


int main() {
    // Build your tree
    Node root, child1, child2;
    build_node_group(&root, "ROOT", 1, std::vector({&child1, &child2}) );

    Node child1_1{ .w = _W, .h = _H, .text = "C1_1" },
        child1_2{ .w = _W, .h = _H, .text = "C1_2" },
        child1_3{ .w = _W, .h = _H, .text = "C1_3" },
        child1_4{ .w = _W, .h = _H, .text = "C1_4" },
        child1_5{ .w = _W, .h = _H, .text = "C1_5" };
    build_node_group(&child1, "R1", 2, 
        std::vector({ &child1_1, &child1_2, &child1_5 }), 
        std::vector({ &child1_3 , &child1_4 }));
 
    Node child2_1 {.w = _W, .h = _H, .text = "C2_1" };
    build_node_group(&child2, "R2", 1, std::vector({ &child2_1 }));

    Node child1_3_1{ .w = _W, .h = _H, .text = "C1_3_1" },
        child1_3_2{ .w = _W, .h = _H, .text = "C1_3_2" },
        child1_3_3{ .w = _W, .h = _H, .text = "C1_3_3" };
    build_node_group(&child1_4, "C1_3", 3,
        std::vector({ &child1_3_1 }),
        std::vector({ &child1_3_2 }),
        std::vector({ &child1_3_3 })
    );


    // Compute layout
    layout::layout(&root);
    render_str(&root, 0);
    std::cout << "ER x:" << root.er->x << std::endl;

    // Node positions are now in `node.x` and `node.y`
    // Render or process as needed…
    Fl_Double_Window win(200, 200, "Draw X");
    Fl_Scroll scroll(0, 0, win.w(), win.h());
    scroll.type(Fl_Scroll::BOTH_ALWAYS);
    //DrawX draw_x(10, 10, win.w() - 20, win.h() - 20);       // put our widget 10 pixels within window edges
    DrawX draw_x(&root, 0, 0, 600, 600);
    scroll.end();
    win.resizable(scroll);
    //
    win.end();
    win.show();
    return(Fl::run());
}
