#pragma once
// The NXP Network Project -- network_node
#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "network_node_struct.h"
#include "network_edge.hpp"
#include "network_draw.hpp"

class DrawXNode : public Fl_Widget {
	Node* node;
	DrawXNodeGroup *pw = nullptr ;
	int groupid = -1;
	DrawXEdge *pedge = nullptr;

	DrawX* g_draw_x = nullptr;
	bool _expanded = false;
	Fl_Color style_fg_color;
	Fl_Font style_font;
	const int style_font_size = _W - _TMARGIN - _TMARGIN;

public:
	DrawXNode(DrawX* g, Node* n, int X, int Y, int W, int H, const char* L = 0) : Fl_Widget(X, Y, W, H, L) {
		align(FL_ALIGN_TOP);
		box(FL_FLAT_BOX);
		color(_COLORBG);
		node = n;
		g_draw_x = g;
		style_fg_color = _COLORFG;
		style_font = FL_COURIER;
	}

	virtual void draw() FL_OVERRIDE;

	int handle(int event);

	void set_pw(DrawXNodeGroup* parent_wgt);
	DrawXNodeGroup* get_pw();
	
	void set_pedge(DrawXEdge* parent_edge);
	DrawXEdge* get_pedge();
	
	void set_groupid(int gid);
	int get_groupid();
	
	Node* get_node();
	
	void set_fg_color(Fl_Color fgc);

	bool expanded();
	void expanded(bool exp);
};
