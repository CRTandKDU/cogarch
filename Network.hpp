#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <cstdint>
#include <cstdarg>
#include <vector>
#include <random>
#include <format>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include "agenda.h"
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
    int _shape = 1;
    // Testing ancillaries
    std::mt19937			_gen;
    std::uniform_int_distribution<> _distr;

    // Constructor
    Network(unsigned short ency_t = 0, const char* title = "");
    ~Network();

    // Methods
    void repopulate();
    void repopulate( hypo_rec_ptr hypo);
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
    void build_node_group(Node* top, int count, ...);
    Node* build_hypo(hypo_rec_ptr hypo, Node* top);
    Node* build_rule(rule_rec_ptr rule, Node* top);
    double  adjustlength(std::string str);
};
#endif
