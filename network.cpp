#include <cstdint>
#include <cstdarg>
#include <vector>

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

void reset_layout(Node* root) {
    // Compute layout
    layout::layout(root);
    render_str(root, 0);
    std::cout << "ER x:" << root->er->x << std::endl;
}


//void test_build_1(Node* root) {
//}
//
//
//void test_build_2(Node* root) {
//}
//
//
//void test_build_root(Node* root) {
//    // build_node_group(root, "ROOT", 1, std::vector({ &child1, &child2 }));
//}

void expand_collapse(Node* root, Node* current, int groupid) {
    std::cout << "Expand or collapse. Root: " << root->text << ", Current: " << current->text << " in group " << groupid << std::endl;
}

int main() {
    // Build your tree
    Node root{ .w = _W, .h = _H, .text = "ROOT" };
    Node child1{ .w = _W, .h = _H, .text = "Untitled-1" }, child2{ .w = _W, .h = _H, .text = "Untitled-2" };
    build_node_group(&root, "ROOT", 1, std::vector({&child1, &child2}) );

	static Node child1_1{ .w = _W, .h = _H, .text = "C1_1" },
		child1_2{ .w = _W, .h = _H, .text = "C1_2" },
		child1_3{ .w = _W, .h = _H, .text = "C1_3" },
		child1_4{ .w = _W, .h = _H, .text = "C1_4" },
		child1_5{ .w = _W, .h = _H, .text = "C1_5" };
	build_node_group(&child1, "R1", 2,
		std::vector({ &child1_1, &child1_2, &child1_5 }),
		std::vector({ &child1_3 , &child1_4 }));

	static Node child2_1{ .w = _W, .h = _H, .text = "C2_1" };
	build_node_group(&child2, "R2", 1, std::vector({ &child2_1 }));

    Node child1_3_1{ .w = _W, .h = _H, .text = "C1_3_1" },
        child1_3_2{ .w = _W, .h = _H, .text = "C1_3_2" },
        child1_3_3{ .w = _W, .h = _H, .text = "C1_3_3" };
    build_node_group(&child1_4, "C1_3", 3,
        std::vector({ &child1_3_1 }),
        std::vector({ &child1_3_2 }),
        std::vector({ &child1_3_3 })
    );

    

    // Node positions are now in `node.x` and `node.y`
    // Render or process as needed…
    Fl_Double_Window win(200, 200, "Draw X");
    Fl_Scroll scroll(0, 0, win.w(), win.h());
    scroll.type(Fl_Scroll::BOTH_ALWAYS);
    reset_layout(&root);
    DrawX g_draw_x(&root, 0, 0, 600, 600);
    scroll.end();
    win.resizable(scroll);
    //
    win.end();
    win.show();
    return(Fl::run());
}
