#include "network_node_group.hpp"
#include "network_node.hpp"
#include "network_draw.hpp"


void DrawX::edge_attach(DrawXNodeGroup* wgt, double* x2ptr, double* y2ptr) {
    DrawXNodeGroup* pw = wgt->get_pw();
    DrawXNode* swgt = nullptr;

    *x2ptr = pw->x() + pw->w();
    //
    if (1 > pw->get_node()->groups.size()) {
        *y2ptr = pw->y() + pw->h() / 2.;
    }
    else {
        if (pw->children() > 0) {
            std::vector< std::vector<Node*> > gvec = pw->get_node()->groups;
            for (short i = 0; i < gvec.size(); i++) {
                if (std::find(gvec[i].begin(), gvec[i].end(), wgt->get_node()) != gvec[i].end()) {
                    swgt = (DrawXNode*)pw->child(i);
                    *y2ptr = swgt->y() + swgt->h() / 2;
                    return;
                }
            }
            // Not found!
            *y2ptr = pw->y() + pw->h() / 2.;
        }
    }
}

void DrawX::get_max_xy(Node* node) {
    if (node->x + node->w > xmax) xmax = node->x + node->w;
    if (node->y + node->h > ymax) ymax = node->y + node->h;
    for (Node* child : node->children) {
        get_max_xy(child);
    }
}

void DrawX::add_children_nodes(Node* node, DrawXNodeGroup* parent_wgt) {
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

        if(0 == node->text.size()){
            for (i = 0; i < node->groups[0].size(); i++) {
                swgt = new DrawXNode(this, node->groups[0][i], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
                swgt->set_groupid(i);
                swgt->set_pw(wgt);
                wgt->add(swgt);
            }
        }
        else {
            for (i = 0; i < node->groups.size(); i++) {
                swgt = new DrawXNode(this, node, net, x() + xnew, y() + ynew + i * _W, wnew, _W);
                swgt->set_groupid(i);
                swgt->set_pw(wgt);
                wgt->add(swgt);
            }
            // Ungraceful check: is this a group of rules (from an hypo expansion), or
            // a group of conditions/actions (from a rule expansion)?
            //if (1 == node->groups.size() && (RULE_MASK == (TYPE_MASK & node->groups[0][0]->sign->len_type))) {
            //    swgt = new DrawXNode(this, node, net, x() + xnew, y() + ynew + i * _W, wnew, _W);
            //    swgt->set_groupid(i);
            //    swgt->set_pw(wgt);
            //    wgt->add(swgt);
            //}
            //else {
            //    // In a LHS. Test whether we have a boolean condition on a hypothesis.
            //    if (node->groups[i][0]->children.size() > 0) {
            //        //wgt_sub = new DrawXNodeGroup(node->groups[i][0], x() + xnew, y() + ynew + i * _W, wnew, _W);
            //        //for (Node* child : node->groups[i][0]->children) {
            //        //    add_children_nodes(child, wgt_sub);
            //        //}
            //        //swgt = new DrawXNode(this, node->groups[i][0], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
            //        //wgt_sub->add(swgt);
            //        //
            //        swgt = new DrawXNode(this, node->groups[i][0], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
            //        swgt->set_groupid(i);
            //        swgt->set_pw(wgt);
            //        wgt->add(swgt);
            //    }
            //    else {
            //        swgt = new DrawXNode(this, node->groups[i][0], net, x() + xnew, y() + ynew + i * _W, wnew, _W);
            //        swgt->set_groupid(i);
            //        swgt->set_pw(wgt);
            //        wgt->add(swgt);
            //    }
            //}
        }
        wgt->set_pw(parent_wgt);
    }
    else {
        std::cout <<
            std::format("\tadd_children_node leaf {}, size={}, type={}", node->text, node->groups.size(), node->sign->len_type & TYPE_MASK) <<
            std::endl;

        // Leaf represented as a single compact
        wgt = new DrawXNodeGroup(node, x() + xnew, y() + ynew, wnew, hnew);
        swgt = new DrawXNode(this, node, net, x() + xnew, y() + ynew, wnew, hnew);
        swgt->set_groupid(0);
        swgt->set_pw(wgt);
        wgt->add(swgt);
        wgt->set_pw(parent_wgt);
    }

    add(wgt);

    if (parent_wgt) {
        DrawXEdge* edge;
        double x1 = wgt->x();
        double y1 = wgt->y() + wgt->h() / 2.;
        double x2 = 0;
        double y2 = 0;
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

        for (Node* child : node->children) {
            add_children_nodes(child, wgt);
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
    add_children_nodes(root, nullptr);
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

