#include <cstdint>
#include <cstdarg>
#include <vector>
#include <random>
#include <format>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>

#include "network_node_struct.h"
#include "network_node_group.hpp"
#include "network_draw.hpp"

//------------------------------------------------------------------------------------------------------

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

void add_node_group(Node* top, std::vector<Node*> group) {
    top->w += _W;
    top->expanded.insert(top->expanded.begin(), false);
    top->groups.insert(top->groups.begin(), group);
    for (Node* child : group) {
        top->children.push_back(child);
        child->parent = top;
    }
}

void build_node_group(Node* top, std::string topname, int count, ...) {
    va_list args;
    va_start(args, count);
    //
    top->w = 0; top->h = _H; top->text = std::string(topname);
    top->_expanded = false;
    for (short i = 0; i < count; i++) {
        std::vector<Node*> group = va_arg(args, std::vector<Node*>);
        add_node_group(top, group);
    }
    va_end(args);
}


void reset_layout(Node* root) {
    // Compute layout
    layout::layout(root);
    render_str(root, 0);
    std::cout << "ER x:" << root->er->x << std::endl;
}


Node* S_root;
std::mt19937 S_gen;
std::uniform_int_distribution<> S_distr;

void expand_collapse(Node* root, Node* current, int groupid) {
	int ngroups ;
	std::vector<Node*> group, ggroup;
	int i, j, ngchildren;
	Node *nnode, *gnode;
    bool b = current->children.size() > 0;
	std::cout << "Expand or collapse. Root: '" << root->text << "' Exp: " << b << ", Current: " << current->text << " in group " << groupid << std::endl;

    if (!b) {
        // First Group
        ngchildren = S_distr(S_gen);
        group = std::vector<Node*>{};
        for (i = 0; i < ngchildren; i++) {
            nnode = new Node{ .w = _W, .h = _H, .text = std::format("NG-{}", i) };
            group.push_back(nnode);
            //
            ngroups = S_distr(S_gen);
            ggroup = std::vector<Node*>{};
            gnode = new Node{ .w = _W, .h = _H, .text = std::format("NG0-{}", i) };
            ggroup.push_back(gnode);
            build_node_group(nnode, std::format("NG-{}", i), 1, ggroup);
            for (j = 1; j < ngroups; j++) {
                ggroup = std::vector<Node*>{};
                gnode = new Node{ .w = _W, .h = _H, .text = std::format("NG0-{}", i) };
                ggroup.push_back(gnode);
                add_node_group(nnode, ggroup);
            }
        }
        build_node_group(current, std::string(std::format("G{}", "0")), 1, group);
    }
    else {
        // Review!
		current->groups.clear();
		for (Node* n : current->children) delete n;
		current->children.clear();
		current->expanded.clear();
    }
}


typedef struct {
    int event_type;
    void* event_data;
} event_t, * event_ptr;
static event_ptr S_event = nullptr;

void post_event(int evt_t, void* data){
    if (S_event) delete S_event;
    S_event = new event_t{ evt_t, data };
};

void cb_idle(void* data) {
    if (S_event) {
        switch (S_event->event_type) {
        case 1:
            DrawX * draw_x = (DrawX*)data;
            draw_x->reset();
            draw_x->redraw();
            break;
        }
        delete S_event;
        S_event = nullptr;
    }
}


int main() {
    int ret;
    // Reference test tree
    Node* root, * child1, * child2;
    root = new Node{ .w = _W, .h = _H, .text = "ROOT" };
    child1 = new Node{ .w = _W, .h = _H, .text = "None-1" };
    child2 = new Node{ .w = _W, .h = _H, .text = "None-2" };
    build_node_group(root, "ROOT", 1, std::vector({child1, child2}) );

	Node* child1_1 = new Node{ .w = _W, .h = _H, .text = "C1_1" };
    Node* child1_2 = new Node{ .w = _W, .h = _H, .text = "C1_2" };
    Node* child1_3 = new Node{ .w = _W, .h = _H, .text = "C1_3" };
    Node* child1_4 = new Node{ .w = _W, .h = _H, .text = "C1_4" };
	Node* child1_5 = new Node{ .w = _W, .h = _H, .text = "C1_5" };
	build_node_group(child1, "R1", 2,
		std::vector({ child1_1, child1_2, child1_5 }),
		std::vector({ child1_3 , child1_4 }));

	Node *child2_1 = new Node{ .w = _W, .h = _H, .text = "C2_1" };
	build_node_group(child2, "R2", 1, std::vector({ child2_1 }));

    Node* child1_3_1 = new Node{ .w = _W, .h = _H, .text = "C1_3_1" };
    Node* child1_3_2 = new Node{ .w = _W, .h = _H, .text = "C1_3_2" };
    Node* child1_3_3 = new Node{ .w = _W, .h = _H, .text = "C1_3_3" };
    build_node_group(child1_4, "C1_3", 3,
        std::vector({ child1_3_1 }),
        std::vector({ child1_3_2 }),
        std::vector({ child1_3_3 })
    );

    S_root = root;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, 5); // define the range
    S_gen = gen;
    S_distr = distr;

    Node *init_root = new Node{ .w = _W, .h = _H, .text = "ROOT", .refnode=root };

    // Node positions are now in `node.x` and `node.y`
    // Render or process as needed…
    Fl_Double_Window win(200, 200, "Draw X");
    Fl_Scroll scroll(0, 0, win.w(), win.h());
    scroll.type(Fl_Scroll::BOTH_ALWAYS);
    reset_layout(init_root);
    DrawX g_draw_x(&scroll, init_root, 0, 0, 1000, 1000);
    scroll.end();
    win.resizable(scroll);
    //
    win.end();
    win.show();
    Fl::add_idle(cb_idle,&g_draw_x);
    ret = Fl::run();
    //
    delete init_root;
    //
    delete child1_3_1;
    delete child1_3_2;
    delete child1_3_3;
    delete child2_1;
    delete child1_1;
    delete child1_2;
    delete child1_3;
    delete child1_4;
    delete child1_5;
    delete child2;
    delete child1;
    delete root;
    S_root = nullptr;
    return ret;
}
