// The NXP Network Project -- network_node
#include "network_node_group.hpp"
#include "network_node.hpp"
#include "Network.hpp"


void DrawXNode::draw()  {
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


int DrawXNode::handle(int event) {
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
    case FL_RELEASE:
        int mouse_but = Fl::event_button();
        int mouse_click = Fl::event_is_click();
        int mouse_x = Fl::event_x();
        int mouse_y = Fl::event_y();
        std::cout << "Click at " << mouse_x << ", " << mouse_y <<
            ". Is_click = " << mouse_click << ", Button = " << mouse_but << std::endl;
        std::cout << "Tof Left " << x() << ", " << y() << "; W-H " << w() << ", " << h() << std::endl;
        //
        if (1 == mouse_click && 1 == mouse_but) {
            Node* r = g_draw_x->get_root();
            net->expand_collapse(r, get_node(), get_groupid());
            net->reset_layout(r);
            post_event(1, (void*)g_draw_x);
        }
        return 1;
        break;
    }
    return ret;
}

void DrawXNode::set_pw(DrawXNodeGroup* parent_wgt) { pw = parent_wgt; }
DrawXNodeGroup* DrawXNode::get_pw() { return pw; }

void DrawXNode::set_pedge(DrawXEdge* parent_edge) { pedge = parent_edge; }
DrawXEdge* DrawXNode::get_pedge() { return pedge; }

void DrawXNode::set_groupid(int gid) { groupid = gid; }
int DrawXNode::get_groupid() { return groupid; }

Node* DrawXNode::get_node() { return node; }

void DrawXNode::set_fg_color(Fl_Color fgc) { style_fg_color = fgc; }

bool DrawXNode::expanded() { return _expanded;  }
void DrawXNode::expanded(bool exp) { _expanded = exp; }
