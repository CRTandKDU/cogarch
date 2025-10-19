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

const std::string CONTAINER("");

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
    std::cout << "reset_layout begin" << std::endl;
    layout::layout(root);
    render_str(root, 0);
    std::cout << "reset_layout ER x:" << root->er->x << std::endl;
}

void Network::expand_collapse(Node* root, Node* current, int groupid) {
    rule_rec_ptr rule = nullptr;
    hypo_rec_ptr hypo = nullptr;
    bwrd_rec_ptr bwrd = nullptr;
    Node *node = nullptr, *cnode = nullptr;
    std::vector<Node*> group;
    int i, idx=0;
    bool b;
    std::string lbl;
    //
    b = current->groups.size() > 0 ? current->expanded[groupid] : current->_expanded;
    std::cout << "Expand or collapse. Root: '" << root->text << "' Exp: " << b << ", Current: " << current->text << " in group " << groupid << std::endl;
    if (current->groups.size() > 0) current->expanded[groupid] = !b; else current->_expanded = !b;
    //
    switch (current->sign->len_type & TYPE_MASK) {
    case RULE_MASK:
        rule = (rule_rec_ptr)current->sign;

        std::cout << std::format("RULE. expand/collapse [in]:\n\t{}\n\t{} #groups\n\t{} #children",
            current->text, current->groups.size(), current->children.size()) <<
            std::endl;

        if( !b ){
            current->children.clear();
            current->groups.clear();
            group.clear();
            node = new Node{ .w = _W, .h = _H, .parent = current, .text = CONTAINER, .sign = (sign_rec_ptr)rule };
            for (i = 0; i < rule->ngetters; i++){
                cond_rec_ptr cond = (cond_rec_ptr)rule->getters[i];
                switch (cond->sign->len_type & TYPE_MASK) {
                case SIGN_MASK:
                    lbl = std::string(cond->sign->str);
                    break;
                case HYPO_MASK:
                    if (0 == cond->in)
                        lbl = std::format("NO {}", std::string(cond->sign->str));
                    else
                        lbl = std::format("YES {}", std::string(cond->sign->str));
                    break;
                case COMPOUND_MASK:
                    lbl = std::string(((compound_rec_ptr)cond->sign)->dsl_expression);
                    lbl.pop_back();
                    break;
                default:
                    lbl = std::string("ERROR: wrong type");
                    break;
                }
                cnode = new Node{
                    .w = _W,
                    .h = adjustlength(lbl),
                    .text = lbl,
                    .sign = (sign_rec_ptr)cond->sign };
                group.push_back(cnode);
                
            }
            build_node_group(node, 1, group);
            current->children.push_back(node);
        }
        else {
            //std::vector<Node*> vec = current->children;
            //for (Node* gnode : group) {
            //    delete_node_group(gnode);
            //    vec.erase(std::remove(vec.begin(), vec.end(), gnode), vec.end());
            //}
            current->children.clear();
        }
        std::cout << std::format("RULE. expand/collapse [out]:\n\t{}\n\t{} #groups\n\t{} #children",
            current->text, current->groups.size(), current->children.size()) <<
            std::endl;

        break;
    //
    case HYPO_MASK:
        // Expanding either the root hypothesis in g_draw_x or a boolean condition
        hypo = (hypo_rec_ptr)current->sign;
        if (current->parent) {
            auto it = std::find(current->parent->groups[0].begin(),
                current->parent->groups[0].end(),
                current);
            if (it != current->parent->groups[0].end())
                idx = it - current->parent->groups[0].begin();
        }


        std::cout << std::format("HYPO. expand/collapse [in]:\n\t{}\n\t{} #groups\n\t{} #children",
            current->text, current->groups.size(),  current->children.size()) <<
            std::endl;

		if( !b ){
            current->children.clear();
            for (i = 0; i < hypo->ngetters; i++) {
                bwrd_rec_ptr bwrd = (bwrd_rec_ptr)hypo->getters[i];
                switch (bwrd->rule->len_type & TYPE_MASK) {
                case RULE_MASK:
                    lbl = std::string(bwrd->rule->str);
                    break;
                default:
                    lbl = std::string("ERROR: wrong type");
                    break;
                }
                cnode = new Node{
                    .w = _W,
                    .h = adjustlength(lbl),
                    .text = lbl,
                    .groupid = idx,
                    .sign = (sign_rec_ptr)bwrd->rule
                };
                // Is `hypo' in LHS?
                if (current->parent) {
                    // Parent rule node at current's parent (LHS "container" node)
                    cnode->parent = current->parent;
                    current->parent->children.push_back(cnode);
                }
                else {
                    // Root (in this design): parent rule node at current node
                    cnode->parent = current;
                    current->children.push_back(cnode);
                }
            }	
            // Is `hypo' in LHS? Sort the parented rule nodes on their rank in LHS
            if (current->parent) {
                std::sort(current->parent->children.begin(),
                    current->parent->children.end(),
                    [](Node* a, Node* b) { return a->groupid > b->groupid; }
                );
            }
		}
		else {
            // Is `hypo' in LHS?
            if (current->parent) {
                std::vector<int> bad_ids = { idx };
                current->parent->children.erase(
                    std::remove_if(
                        current->parent->children.begin(),
                        current->parent->children.end(),
                        [&bad_ids](const Node *item) { return std::find(bad_ids.begin(), bad_ids.end(), item->groupid) != bad_ids.end(); }),
                    current->parent->children.end());
            }
            else {
                current->children.clear();
            }
		}

		std::cout << std::format("HYPO. expand/collapse [out]:\n\t{}\n\t{} #groups\n\t{} #children",
			current->text, current->groups.size(), 
            current->children.size()) <<
			std::endl;

		break;
    case SIGN_MASK:
    case COMPOUND_MASK:
	default:
		break;
	}
}


//------------------------------------------------------------------------------------------------------
void Network::render_str(Node* top, int level) {
    std::string s = std::string("").append(level, ' ');
    std::cout << s.append(std::format("{}: x = {}, y = {}, w = {}, h = {}", 
        top->text, top->x, top->y, top->w, top->h)) << std::endl;
    for (Node* child : top->children) {
        render_str(child, level + 2);
    }
}

void Network::build_node_plain(Node* top, std::string topname, int count, ...) {
    va_list args;
    va_start(args, count);
    //
    top->h = adjustlength(topname);
    top->w = _W; 
    top->text = std::string(topname);
    for (short i = 0; i < count; i++) {
        Node* child = va_arg(args, Node*);
        top->children.push_back(child);
        child->parent = top;
    }
    va_end(args);
}

void Network::add_node_group(Node* top, std::vector<Node*> group) {
    int lcur = 0;
    //top->w += _W;
    top->expanded.insert(top->expanded.begin(), false);
    top->groups.insert(top->groups.begin(), group);
    for (Node* child : group) {
        // top->children.push_back(child);
        lcur = adjustlength(child->text);
        if (lcur > top->h)
            top->h = lcur;
        child->parent = top;
        top->w += _W;
    }
}

void Network::build_node_group(Node* top, std::string topname, int count, ...) {
    double len = 0, lcur=0;
    va_list args;
    va_start(args, count);
    //
    top->w = 0; top->h = _H; 
    top->text = std::string(topname);
    top->_expanded = false;

    for (short i = 0; i < count; i++) {
        std::vector<Node*> group = va_arg(args, std::vector<Node*>);
        for (Node* n : group) {
            lcur = adjustlength(n->text);
            if (lcur > len) len = lcur;
        }
        add_node_group(top, group);
    }
    va_end(args);
    top->h = len;
}

void Network::build_node_group(Node* top, int count, ...) {
    double len = 0, lcur = 0;
    va_list args;
    va_start(args, count);
    // Reduced to count == 1 
    top->w = 0; top->h = _H;
    top->_expanded = false;

    for (short i = 0; i < count; i++) {
        std::vector<Node*> group = va_arg(args, std::vector<Node*>);
        for (Node* n : group) {
            lcur = adjustlength(n->text);
            if (lcur > len) len = lcur;
        }
        add_node_group(top, group);
    }
    va_end(args);
    top->h = len;
}

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

double Network::adjustlength(std::string str) {
    int dx, dy, ho, wo;
    double len, ret;
    fl_font(FL_COURIER, 12);
    fl_text_extents(str.c_str(), dx, dy, wo, ho);
    len = (double) ( wo + _LMARGIN + _LMARGIN );
    switch (_shape) {
    case 1:
        ret = len < _H ? _H : len;
        break;
    case 0:
    default:
        ret = _H;
        break;
    }
    return ret;
}

Node* Network::build_hypo(hypo_rec_ptr hypo, Node* top) {
    Node* node;
    bwrd_rec_ptr bwrd;
    // Rules for hypo
    std::vector<Node*> vec_rules;
    for (unsigned short i = 0; i < hypo->ngetters; i++) {
        bwrd = (bwrd_rec_ptr)hypo->getters[i];
        node = new Node{ .w = _W, .h = adjustlength(std::string(bwrd->rule->str)), .text = bwrd->rule->str, .sign = (sign_rec_ptr)bwrd->rule };
        vec_rules.push_back(node);
    }
    build_node_group(top, 1, vec_rules);
    return top;
}

Node* Network::build_rule(rule_rec_ptr rule, Node* top) {
    Node* node;
    cond_rec_ptr cond;
    // Rules for hypo
    std::vector<Node*> vec_lhs, vec_rhs;
    for (unsigned short i = 0; i < rule->ngetters; i++) {
        cond = (cond_rec_ptr)rule->getters[i];
        node = new Node{ .w = _W, .h = adjustlength(std::string(cond->sign->str)), 
            .text = cond->sign->str, .sign = (sign_rec_ptr)cond->sign };
        vec_lhs.push_back(node);
    }
    for (unsigned short i = 0; i < rule->nrhs; i++) {
        node = new Node{ .w = _W, .h = adjustlength(std::string((char*)rule->rhs[i])), 
            .text = (char*)rule->rhs[i] };
        vec_rhs.push_back(node);
    }

    build_node_group(top, 2, vec_lhs, vec_rhs);
    return top;
}

void Network::repopulate(hypo_rec_ptr hypo) {
    Node* root = new Node{ .w = _W, .h = adjustlength(hypo->str), .text = hypo->str, .sign = hypo };
    init_root = _root = root;
    update();
    post_event(1, g_draw_x);
}

void Network::repopulate() {
    // Reference test tree
 //   Node* root, * child1, * child2;
 //   root = new Node{ .w = _W, .h = _H, .text = "ROOT" };
 //   child1 = new Node{ .w = _W, .h = _H, .text = "None-1" };
 //   child2 = new Node{ .w = _W, .h = _H, .text = "None-2" };
 //   build_node_group(root, "ROOT", 1, std::vector({child1, child2}) );

	//Node* child1_1 = new Node{ .w = _W, .h = _H, .text = "C1_1" };
 //   Node* child1_2 = new Node{ .w = _W, .h = _H, .text = "C1_2" };
 //   Node* child1_3 = new Node{ .w = _W, .h = _H, .text = "C1_3" };
 //   Node* child1_4 = new Node{ .w = _W, .h = _H, .text = "C1_4" };
	//Node* child1_5 = new Node{ .w = _W, .h = _H, .text = "C1_5" };
	//build_node_group(child1, "R1", 2,
	//	std::vector({ child1_1, child1_2, child1_5 }),
	//	std::vector({ child1_3 , child1_4 }));

	//Node *child2_1 = new Node{ .w = _W, .h = _H, .text = "C2_1" };
	//build_node_group(child2, "R2", 1, std::vector({ child2_1 }));

 //   Node* child1_3_1 = new Node{ .w = _W, .h = _H, .text = "C1_3_1" };
 //   Node* child1_3_2 = new Node{ .w = _W, .h = _H, .text = "C1_3_2" };
 //   Node* child1_3_3 = new Node{ .w = _W, .h = _H, .text = "C1_3_3" };
 //   build_node_group(child1_4, "C1_3", 3,
 //       std::vector({ child1_3_1 }),
 //       std::vector({ child1_3_2 }),
 //       std::vector({ child1_3_3 })
 //   );

 //   _root = root;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, 5); // define the range
    _gen = gen;
    _distr = distr;

    init_root = new Node{ .w = _W, .h = _H };
    _root = init_root;
    build_node_group(init_root, "ROOT", 1, std::vector({ new Node{.w = _W, .h = _H, .text = "root" } }));

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
    reset_layout(_root);
    g_draw_x->set_root(_root);
}

// Constructor
Network::Network(unsigned short ency_t, const char* title) : Fl_Double_Window(400, 400, "Network") {
    ency_t = ency_t;
    title = title;
    //
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, 5); // define the range
    _gen = gen;
    _distr = distr;
    //
    _scroll = new Fl_Scroll(0, 0, w(), h());
    _scroll->type(Fl_Scroll::BOTH_ALWAYS);
    g_draw_x = new DrawX(_scroll, init_root, this, 0, 0, 1000, 1000);
    _scroll->end();
    resizable(_scroll);
    end();
    Fl::add_idle(cb_idle, g_draw_x);
}

Network::~Network() { }

