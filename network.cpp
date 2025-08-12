#include <vector>
#include <string>
#include <iostream>
#include <format>
#include <array>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>

#include "layout.hpp"

constexpr auto _W = 20.;
constexpr auto _H = 100.;


struct Node {
    double x, y, w, h;
    double prelim = 0, mod = 0, shift = 0, change = 0;
    Node* parent = nullptr;
    std::vector<Node*> children;
    Node* tl = nullptr, * tr = nullptr, * el = nullptr, * er = nullptr;
    double msel = 0, mser = 0;
    // Clientdata
    std::string text;
};

void render_str(Node* top, int level) {
    std::string s = std::string("").append(level, ' ');
    std::cout << s.append(std::format("{}: x = {}, y = {}", top->text, top->x, top->y)) << std::endl;
    for (Node* child : top->children) {
        render_str(child, level + 2);
    }
}

class DrawX : public Fl_Widget {
    std::array<double,2> render( Node *node) {
        double xnew, ynew, wnew, hnew;
        fl_color(FL_BLACK);
        xnew = node->y; ynew = node->x + node->w;
        wnew = node->h; hnew = node->w;
        fl_rect(x() + xnew, y() + ynew, wnew, hnew);
        std::array<double, 2> source = { xnew+wnew, ynew + hnew / 2. };
        for (Node* child : node->children) {
            std::array<double, 2> dest_child = render(child);
            fl_line(source[0], source[1], dest_child[0], dest_child[1]);
        }
        std::array<double, 2> dest = {xnew, ynew+hnew/2.};
        return dest;
    }

public:
    Node* root = nullptr;

    DrawX(int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
        align(FL_ALIGN_TOP);
        box(FL_FLAT_BOX);
        color(FL_WHITE);
    }
    
    virtual void draw() FL_OVERRIDE {
        std::array<double, 2> root_pt;
        // Draw background - a white filled rectangle
        fl_color(FL_WHITE); fl_rectf(x(), y(), w(), h());
        // Draw black 'X' over base widget's background
        if (root) root_pt =render(root);
        //fl_color(FL_BLACK);
        //int x1 = x(), y1 = y();
        //int x2 = x() + w() - 1, y2 = y() + h() - 1;
        //fl_line(x1, y1, x2, y2);
        //fl_line(x1, y2, x2, y1);
    }
};


int main() {
    // Build your tree
    Node root{ .w = _W, .h = _H, .text = "root" }, child1{ .w = _W, .h = _H, .text = "C1" }, child2{ .w = _W, .h = _H, .text = "C2" };
    root.children = { &child1, &child2 };
    child1.parent = &root;
    child2.parent = &root;
    // … fill in w, h for each node …

    // Compute layout
    layout::layout(&root);

    // Node positions are now in `node.x` and `node.y`
    // Render or process as needed…
    Fl_Double_Window win(200, 200, "Draw X");
    Fl_Scroll scroll(0, 0, win.w(), win.h());
    scroll.type(Fl_Scroll::BOTH_ALWAYS);
    //DrawX draw_x(10, 10, win.w() - 20, win.h() - 20);       // put our widget 10 pixels within window edges
    DrawX draw_x(0, 0, 600, 600);
    draw_x.root = &root;
    scroll.end();
    win.resizable(scroll);
    //
    win.end();
    win.show();
    return(Fl::run());
}
