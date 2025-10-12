// #pragma once
#ifndef NETWORK_NODE_STRUCT_H
#define NETWORK_NODE_STRUCT_H

#include <cstdint>
#include <cstdarg>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <format>

#include "layout.hpp"

constexpr auto _W = 20.;
constexpr auto _H = 100.;
constexpr auto _LMARGIN = 2;
constexpr auto _TMARGIN = 1;

const uint32_t _COLORBG = 0xFFFFFF00;
const uint32_t _COLORFG = 0x00000000;
const uint32_t _SELECTION_COLOR = 0x9575CD00;
const uint32_t _HOVER_COLORBG = 0x18549E00;
const uint32_t _HOVER_COLORFG = 0xFFFFFF00;


// Forward declarations
class Network;

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
    std::vector< bool > expanded = {};
    bool _expanded = false;
};

typedef struct {
    int event_type;
    void* event_data;
} event_t, * event_ptr;
static event_ptr S_event = nullptr;



void post_event(int evt_t, void* data);

#endif
