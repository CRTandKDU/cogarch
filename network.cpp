// #include <cstdint>
// #include <cstdarg>
// #include <vector>
// #include <random>
// #include <format>

// #include <FL/Fl_Double_Window.H>
// #include <FL/Fl_Scroll.H>

// #include "network_node_struct.h"
// #include "network_node_group.hpp"
// #include "network_draw.hpp"

#include "Network.hpp"

void post_event(int evt_t, void* data) {
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

void Network::reset_layout(Node* root) {
    // Compute layout
    layout::layout(root);
    render_str(root, 0);
    std::cout << "ER x:" << root->er->x << std::endl;
}


void Network::expand_collapse(Node* root, Node* current, int groupid) {
    int ngroups;
    std::vector<Node*> group, ggroup;
    int i, j, ngchildren;
    Node* nnode;
    bool b = current->groups.size() > 0 ? current->expanded[groupid] : current->_expanded;
    std::cout << "Expand or collapse. Root: '" << root->text << "' Exp: " << b << ", Current: " << current->text << " in group " << groupid << std::endl;
    if (current->groups.size() > 0) current->expanded[groupid] = !b;
    else current->_expanded = !b;
    //
    group = current->groups[groupid];
    if (!b) {
        int pos = search_insertion_index(current, groupid);
        //
        for (Node* gnode : group) {
            ngchildren = _distr(_gen);
            for (i = 0; i < ngchildren; i++) {
                ggroup = std::vector<Node*>{};
                ngroups = _distr(_gen);
                for (j = 0; j < ngroups; j++) {
                    ggroup.push_back(new Node{ .w = _W, .h = _H,
                        .text = std::format("{}{}-{}", gnode->text, i, j) });
                }
                if (0 == i) {
                    build_node_group(gnode, gnode->text, 1, ggroup);
                }
                else {
                    add_node_group(gnode, ggroup);
                }
            }
            current->children.insert(current->children.begin() + pos, gnode);
            //current->children.push_back(gnode);
        }
    }
    else {
        std::vector<Node*> vec = current->children;
        for (Node* gnode : group) {
            delete_node_group(gnode);
            vec.erase(std::remove(vec.begin(), vec.end(), gnode), vec.end());
        }
        current->children = vec;
    }
}


//------------------------------------------------------------------------------------------------------

void Network::render_str(Node* top, int level) {
    std::string s = std::string("").append(level, ' ');
    std::cout << s.append(std::format("{}: x = {}, y = {}", top->text, top->x, top->y)) << std::endl;
    for (Node* child : top->children) {
        render_str(child, level + 2);
    }
}


void Network::build_node_plain(Node* top, std::string topname, int count, ...) {
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

void Network::add_node_group(Node* top, std::vector<Node*> group) {
    top->w += _W;
    top->expanded.insert(top->expanded.begin(), false);
    top->groups.insert(top->groups.begin(), group);
    for (Node* child : group) {
        // top->children.push_back(child);
        child->parent = top;
    }
}


void Network::build_node_group(Node* top, std::string topname, int count, ...) {
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




// Node* S_root;
// std::mt19937 S_gen;
// std::uniform_int_distribution<> S_distr;

int Network::search_insertion_index(Node* current, int groupid) {
    int pos = 0, idg = 0;
    for (pos = 0; pos < current->children.size(); pos++) {
        for (idg = 0; idg < current->groups.size(); idg++) {
            if (current->groups[idg].end() !=
                std::find(current->groups[idg].begin(), current->groups[idg].end(), current->children[pos])) {
                if (idg <= groupid) return pos;
            }
        }
    }
    return current->children.size();
}

void Network::delete_node_group(Node* top) {
    for (std::vector<Node*> g : top->groups) {
        for (Node* n : g) {
            delete_node_group(n);
            n->children.clear();
        }
        g.clear();
    }
    top->groups.clear();
    top->children.clear();
}



// typedef struct {
//     int event_type;
//     void* event_data;
// } event_t, * event_ptr;
// static event_ptr S_event = nullptr;

// void post_event(int evt_t, void* data){
//     if (S_event) delete S_event;
//     S_event = new event_t{ evt_t, data };
// };

// void cb_idle(void* data) {
//     if (S_event) {
//         switch (S_event->event_type) {
//         case 1:
//             DrawX * draw_x = (DrawX*)data;
//             draw_x->reset();
//             draw_x->redraw();
//             break;
//         }
//         delete S_event;
//         S_event = nullptr;
//     }
// }



void Network::repopulate() {
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

    _root = root;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, 5); // define the range
    _gen = gen;
    _distr = distr;

    init_root = new Node{ .w = _W, .h = _H };
    build_node_group(init_root, "ROOT", 1, std::vector({ new Node{.w = _W, .h = _H, .text = "root" } }));

    // Node positions are now in `node.x` and `node.y`
    //// Render or process as needed…
    //Fl_Double_Window win(200, 200, "Draw X");
    //Fl_Scroll scroll(0, 0, win.w(), win.h());
    //scroll.type(Fl_Scroll::BOTH_ALWAYS);
    //reset_layout(init_root);
    //DrawX g_draw_x(&scroll, init_root, this, 0, 0, 1000, 1000);
    //scroll.end();
    //win.resizable(scroll);
    ////
    //win.end();
    //win.show();
    //Fl::add_idle(cb_idle,&g_draw_x);
    //ret = Fl::run();
    //
    //delete init_root;
    ////
    //delete child1_3_1;
    //delete child1_3_2;
    //delete child1_3_3;
    //delete child2_1;
    //delete child1_1;
    //delete child1_2;
    //delete child1_3;
    //delete child1_4;
    //delete child1_5;
    //delete child2;
    //delete child1;
    //delete root;
    //_root = nullptr;

}

void Network::update() {
    if (_scroll) delete _scroll;
    _scroll = new Fl_Scroll(0, 0, w(), h());
    _scroll->type(Fl_Scroll::BOTH_ALWAYS);
    reset_layout(init_root);
    if (g_draw_x) delete g_draw_x;
    g_draw_x = new DrawX(_scroll, init_root, this, 0, 0, 1000, 1000);
    _scroll->end();
    resizable(_scroll);
    Fl::add_idle(cb_idle, g_draw_x);
}

// Constructor
Network::Network(unsigned short ency_t, const char* title) : Fl_Double_Window(400, 400, "Network") {
    ency_t = ency_t;
    title = title;
}

Network::~Network() { }

