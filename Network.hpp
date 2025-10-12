#ifndef NETWORK_HPP
#define NETWORK_HPP

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

//----------------------------------------------------------------------
// class Network
//----------------------------------------------------------------------
class Network : public Fl_Double_Window {

public:
    // Members
    unsigned short ency_t;
    const char* title;
    Node* _root;
    Node* init_root;
    Fl_Scroll* _scroll = nullptr;
    DrawX* g_draw_x = nullptr;
    std::mt19937			_gen;
    std::uniform_int_distribution<> _distr;

    // Constructor
    Network(unsigned short ency_t = 0, const char* title = "");
    ~Network();

    // Methods
    void repopulate();
    void update();
    void reset_layout(Node* root);
    int search_insertion_index(Node* current, int groupid);
    void expand_collapse(Node* root, Node* current, int groupid);

private:
    void render_str(Node* top, int level);
    void build_node_plain(Node* top, std::string topname, int count, ...);
    void add_node_group(Node* top, std::vector<Node*> group);
    void delete_node_group(Node* top);
    void build_node_group(Node* top, std::string topname, int count, ...);

};
#endif
