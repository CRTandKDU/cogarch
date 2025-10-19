#include "network_node_group.hpp"
#include "network_node.hpp"
#include "network_draw.hpp"

void DrawX::edge_attach(DrawXNode* swgt, double* x2ptr, double* y2ptr) {
    Fl_Widget* pw = (Fl_Widget*)swgt->get_pw();

	*x2ptr = pw->x() + pw->w();
	*y2ptr = pw->y() + pw->h() / 2.;
}

void DrawX::edge_attach(DrawXNodeGroup* wgt, double* x2ptr, double* y2ptr) {
    Fl_Widget* pw = wgt->get_pw();

    *x2ptr = pw->x() + pw->w();
    *y2ptr = pw->y() + pw->h() / 2.;
}

void DrawX::get_max_xy(Node* node) {
    if (node->x + node->w > xmax) xmax = node->x + node->w;
    if (node->y + node->h > ymax) ymax = node->y + node->h;
    for (Node* child : node->children) {
        get_max_xy(child);
    }
}

void DrawX::add_children_nodes(Node* node, DrawXNode* parent_wgt) {
    if (nullptr == node) return;

    double xnew, ynew, wnew, hnew;
    short i;
    // Transform coordinates
    xnew = node->y; ynew = xmax - node->x - node->w;
    wnew = node->h; hnew = node->w;
    //
    DrawXNodeGroup* wgt = nullptr;
    DrawXNode* swgt = nullptr;
    DrawXNodeGroup* wgt_sub = nullptr;

    std::cout << std::format("Entering add_children_nodes: {}, groups size {}, children {}",
        node->text, node->groups.size(), node->children.size()) << std::endl;

    if (node->groups.size() > 0) {
        // Node is a compact
        wgt = new DrawXNodeGroup(node, x() + xnew, y() + ynew, wnew, hnew);
        std::cout <<
            std::format("\tadd_children_node compact <{}> ({} chars), size={}, type={}",
                node->text, node->text.size(),
                node->groups.size(),
                node->sign ? node->sign->len_type & TYPE_MASK : -1) <<
            std::endl;

        for (i = 0; i < node->groups[0].size(); i++) {
            swgt = new DrawXNode(this, node->groups[0][i], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
            swgt->set_groupid(i);
            swgt->set_pw(wgt);
            wgt->add(swgt);
        }
        wgt->set_pw(parent_wgt);
        add(wgt);
    }
    else {
        std::cout <<
            std::format("\tadd_children_node leaf {}, size={}, type={}",
                node->text, node->groups.size(), node->sign->len_type & TYPE_MASK) <<
            std::endl;

        swgt = new DrawXNode(this, node, net, x() + xnew, y() + ynew, wnew, hnew);
        swgt->set_pw(parent_wgt);
        add(swgt);
    }

    if (parent_wgt) {
        DrawXEdge* edge;
        double x1, x2 = 0;
        double y1, y2 = 0;
		if (node->groups.size() > 0) {
			x1 = wgt->x();
			y1 = wgt->y() + wgt->h() / 2.;
            edge_attach(wgt, &x2, &y2);
            if (y1 < y2) {
				edge = new DrawXEdge(_BOT_LEFT, x2, y1, x1 - x2, y2 - y1);
			}
			else {
				edge = new DrawXEdge(_TOP_LEFT, x2, y2, x1 - x2, y1 - y2);
			}
			add(edge);
			wgt->set_pedge(edge);
		}
		else {
			x1 = swgt->x();
			y1 = swgt->y() + swgt->h() / 2.;
            edge_attach(swgt, &x2, &y2);
			if (y1 < y2) {
				edge = new DrawXEdge(_BOT_LEFT, x2, y1, x1 - x2, y2 - y1);
			}
			else {
				edge = new DrawXEdge(_TOP_LEFT, x2, y2, x1 - x2, y1 - y2);
			}
			add(edge);
			swgt->set_pedge(edge);
		}
    }

    if (node->groups.size() > 0) {
        //for (Node* n : node->groups[0]) {
        //    for (Node* child : n->children) {
        //        add_children_nodes(child, wgt);
        //    }
        //}
        for (Node* child : node->children) {
            if (child) add_children_nodes(child, wgt);
        }
    }
    else {
        for (Node* child : node->children) {
            if(child) add_children_nodes(child, swgt);
        }
    }
}


void DrawX::add_children_nodes(Node* node, DrawXNodeGroup* parent_wgt) {
    if (nullptr == node) return;

    double xnew, ynew, wnew, hnew;
    short i;
    // Transform coordinates
    xnew = node->y; ynew = xmax - node->x - node->w;
    wnew = node->h; hnew = node->w;
    //
    DrawXNodeGroup* wgt = nullptr;
    DrawXNode* swgt = nullptr;
    DrawXNodeGroup* wgt_sub = nullptr;

    std::cout << std::format("Entering add_children_nodes: {}, groups size {}, children {}", 
        node->text, node->groups.size(), node->children.size())<< std::endl;

	if (node->groups.size() > 0) {
		// Node is a compact
		wgt = new DrawXNodeGroup(node, x() + xnew, y() + ynew, wnew, hnew);
		std::cout <<
			std::format("\tadd_children_node compact <{}> ({} chars), size={}, type={}",
				node->text, node->text.size(),
				node->groups.size(),
				node->sign ? node->sign->len_type & TYPE_MASK : -1) <<
			std::endl;

		for (i = 0; i < node->groups[0].size(); i++) {
			swgt = new DrawXNode(this, node->groups[0][i], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
			swgt->set_groupid(i);
			swgt->set_pw(wgt);
			wgt->add(swgt);
		}
		wgt->set_pw(parent_wgt);
        add(wgt);
	}
    else {
        std::cout <<
            std::format("\tadd_children_node leaf {}, size={}, type={}", 
                node->text, node->groups.size(), node->sign->len_type & TYPE_MASK) <<
            std::endl;

        swgt = new DrawXNode(this, node, net, x() + xnew, y() + ynew, wnew, hnew);
        swgt->set_pw(parent_wgt);
        add(swgt);
    }
    
    if (parent_wgt) {
        DrawXEdge* edge;
        double x2 = 0;
        double y2 = 0;
        if (node->groups.size() > 0) {
            double x1 = wgt->x();
            double y1 = wgt->y() + wgt->h() / 2.;
            edge_attach(wgt, &x2, &y2);
            if (y1 < y2) {
                edge = new DrawXEdge(_BOT_LEFT, x2, y1, x1 - x2, y2 - y1);
            }
            else {
                edge = new DrawXEdge(_TOP_LEFT, x2, y2, x1 - x2, y1 - y2);
            }
            add(edge);
            wgt->set_pedge(edge);
        }
        else {
            double x1 = swgt->x();
            double y1 = swgt->y() + swgt->h() / 2.;
            if (parent_wgt->get_node()->groups.size() > 0) {
                x2 = parent_wgt->x() + parent_wgt->w();
                y2 = parent_wgt->y() + _W * (.5 + node->groupid); // pw->h() / 2.;

            }
            else {
                edge_attach(swgt, &x2, &y2);
            }
            if (y1 < y2) {
                edge = new DrawXEdge(_BOT_LEFT, x2, y1, x1 - x2, y2 - y1);
            }
            else {
                edge = new DrawXEdge(_TOP_LEFT, x2, y2, x1 - x2, y1 - y2);
            }
            add(edge);
            swgt->set_pedge(edge);
        }
    }

    if (node->groups.size() > 0) {
        //for (Node* n : node->groups[0]) {
        //    for (Node* child : n->children) {
        //        add_children_nodes(child, wgt);
        //    }
        //}
        for (Node* child : node->children) {
            if (child) add_children_nodes(child, wgt);
        }
    }
    else {
        for (Node* child : node->children) {
            if (child) add_children_nodes(child, swgt);
        }
    }
}

 void DrawX::draw()  {
    // Draw background - a white filled rectangle
    fl_color(color()); fl_rectf(x(), y(), w(), h());
    //
    draw_children();
}

void DrawX::reset() {
    xmax = 0; ymax = 0;
    get_max_xy(root);
    // Build depth-first traversal list of children node-widgets
    clear();
    add_children_nodes(root, (DrawXNode *)nullptr);
    std::cout << "XYmax: " << xmax << ", " << ymax << std::endl;
}

Node* DrawX::get_root() { return root; };

void  DrawX::set_root(Node* r) {
    root = r;
    reset();
}

Fl_Scroll* DrawX::scroll() {
    return _scroll;
};

void DrawX::scroll(Fl_Scroll* sc) {
    _scroll = sc;
}

