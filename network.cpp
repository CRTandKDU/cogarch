#include <cstdint>
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


struct Node {
    double x, y, w, h;
    double prelim = 0, mod = 0, shift = 0, change = 0;
    Node* parent = nullptr;
    std::vector<Node*> children;
    Node* tl = nullptr, * tr = nullptr, * el = nullptr, * er = nullptr;
    double msel = 0, mser = 0;
    // Clientdata
    std::string text;
    std::vector< std::vector<Node*> > groups;
};

void render_str(Node* top, int level) {
    std::string s = std::string("").append(level, ' ');
    std::cout << s.append(std::format("{}: x = {}, y = {}", top->text, top->x, top->y)) << std::endl;
    for (Node* child : top->children) {
        render_str(child, level + 2);
    }
}

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

class DrawXNode : public Fl_Widget {
    Node* node;
    DrawXNode* pw;
    Fl_Color style_fg_color;
    Fl_Font style_font;
    const int style_font_size = _W - _TMARGIN - _TMARGIN;
    short subdivision;

public:
    DrawXNode(Node* n, int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(_COLORBG);
        node = n;
        style_fg_color = _COLORFG;
        style_font = FL_COURIER;
        subdivision = -1;
    }

    virtual void draw() FL_OVERRIDE {
        double hsubs = h() / _W;
        // Draw background - a white filled rectangle
        fl_color(color()); fl_rectf(x(), y(), w(), h());
        if (subdivision > 0) {
            fl_color(_HOVER_COLORBG);
            fl_rectf(x(), y()+_W*subdivision, w(), hsubs);
        }
        // Draw node
        if( node ){
            fl_color(FL_BLACK);
            fl_rect(x(), y(), w(), h());
            fl_color(style_fg_color);
            fl_font(style_font, style_font_size);
            fl_draw(node->text.c_str(), _LMARGIN + x(), y(), w(), h(),
                FL_ALIGN_TOP_LEFT, nullptr, 1, 0);
        }
    }

    int handle(int event)  {
        int ret = Fl_Widget::handle(event);
        switch (event) {
        case FL_ENTER:
            color(_HOVER_COLORBG);
            set_subdivision(-1);
            set_fg_color(_HOVER_COLORFG);
            redraw();
            return 1;
            break;
        case FL_LEAVE:
            color(_COLORBG);
            set_subdivision(-1);
            set_fg_color(_COLORFG);
            redraw();
            return 1;
            break;
        }
        return ret;
    }

	void set_pw(DrawXNode* parent_wgt) { pw = parent_wgt; }
    DrawXNode*  get_pw() { return pw; }

    Node* get_node() { return node; }

    short get_subdivision() { return subdivision; }
    void  set_subdivision(short n) { subdivision = n; }

    void set_fg_color(Fl_Color fgc) { style_fg_color = fgc; }
};


class DrawX : public Fl_Group {
    Node* root = nullptr;

    void edge_attach(DrawXNode *wgt, double* x2ptr, double* y2ptr) {
        double yinc = 0;
        DrawXNode* pw = wgt->get_pw();
        *x2ptr = pw->x() + pw->w();

        if (0 == pw->get_node()->groups.size()) {
            *y2ptr = pw->y() + pw->h() / 2.;
        }
        else {
            for (short i = 0; i < pw->get_node()->groups.size(); i++) {
                if (std::find(pw->get_node()->groups[i].begin(),
                    pw->get_node()->groups[i].end(),
                    wgt->get_node()) != pw->get_node()->groups[i].end()) {
                    yinc = pw->h() / pw->get_node()->groups.size();
                    *y2ptr = pw->y() + i * yinc + yinc / 2;
                    return;
                }
            }
            // Not found!
            *y2ptr = pw->y() + pw->h() / 2.;
        }
    }

    void add_children_nodes(Node* node, DrawXNode *parent_wgt) {
        double xnew, ynew, wnew, hnew;
        const int font_size = _W - _TMARGIN - _TMARGIN;
        // Transform coord.
        xnew = node->y; ynew = node->x + node->w;
        wnew = node->h; hnew = node->w;
        //
        DrawXNode *wgt = new DrawXNode(node, x() + xnew, y() + ynew, wnew, hnew);
        wgt->set_pw(parent_wgt);
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
        // Build depth-first traversal list of children node-widgets
        add_children_nodes(root, nullptr);
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
    Node root{ .w = _W, .h = _H, .text = "root" }, 
        child1{ .w = _W, .h = _H, .text = "C1" }, 
        child2{ .w = _W, .h = _H, .text = "C2" };
    root.children = { &child1, &child2 };
    child1.parent = &root;
    child2.parent = &root;

    Node child1_1{ .w = _W, .h = _H, .text = "C1_1" },
        child1_2{ .w = _W, .h = _H, .text = "C1_2" },
        child1_3{ .w = _W, .h = _H, .text = "C1_3" },
        child1_4{ .w = _W, .h = _H, .text = "C1_4" };
    child1.children = { &child1_1, &child1_2 , &child1_3 , &child1_4 };
    child1.groups = { {&child1_1, &child1_2}, {&child1_3 , &child1_4} };
    child1.w = child1.groups.size() * _W;
    child1_1.parent = &child1;
    child1_2.parent = &child1;
    child1_3.parent = &child1;
    child1_4.parent = &child1;
 

    // Compute layout
    layout::layout(&root);

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
