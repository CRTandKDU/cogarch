#pragma once

#include <string>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>

#include "agenda.h"
#include "hello.h"

#define MAX_ROWS 300
#define MAX_COLS 2

// Derive a class from Fl_Table
class EncyTable : public Fl_Table {
    enum item;
    item item_t;
    std::vector<sign_rec_ptr> data;


    // Draw the row/col headings
    //    Make this a dark thin upbox with the text inside.
    //
    void DrawHeader(const char* s, int X, int Y, int W, int H) {
        fl_push_clip(X, Y, W, H);
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
        fl_color(FL_BLACK);
        fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
        fl_pop_clip();
    }
    // Draw the cell data
    //    Dark gray text on white background with subtle border
    //
    void DrawData(const char* s, int X, int Y, int W, int H) {
        fl_push_clip(X, Y, W, H);
        // Draw cell bg
        fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
        // Draw cell data
        fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_LEFT);
        // Draw box border
        fl_color(color()); fl_rect(X, Y, W, H);
        fl_pop_clip();
    }
    // Handle drawing table's cells
    //     Fl_Table calls this function to draw each visible cell in the table.
    //     It's up to us to use FLTK's drawing functions to draw the cells the way we want.
    //
    void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) FL_OVERRIDE {
        static char s[40];
        switch (context) {
        case CONTEXT_STARTPAGE:                   // before page is drawn..
            fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
            return;
        case CONTEXT_COL_HEADER:                  // Draw column headers
            switch (COL) {
            case 0:
                switch (item_t) {
                case SIGN:
                    sprintf(s, "Sign");
                    break;
                case HYPO:
                    sprintf(s, "Hypothesis");
                    break;
                case RULE:
                    sprintf(s, "Rule");
                    break;
                }
                break;
            case 1:
                sprintf(s, "Value");
                break;
            default:
                *s = 0;
                break;
            }
            DrawHeader(s, X, Y, W, H);
            return;
        case CONTEXT_ROW_HEADER:                  // Draw row headers
            sprintf(s, "%03d:", ROW);             // "001:", "002:", etc
            DrawHeader(s, X, Y, W, H);
            return;
        case CONTEXT_CELL:                        // Draw data in cells
            switch (COL) {
            case 0:
                DrawData(data.at(ROW)->str, X, Y, W, H);
                break;
            case 1:
                DrawData(repl_val_repr(&(data.at(ROW)->val)).c_str(), X, Y, W, H);
                break;
            }
            return;
        default:
            return;
        }
    }

public:
    enum item {
        SIGN,
        HYPO,
        RULE,
    };
    sign_rec_ptr top;

    void clear() { data.clear(); }

    int fill(sign_rec_ptr first) {
        sign_rec_ptr sign;
        int row = 0;
        clear();
        top = first;
        for (sign = top, row = 0; sign; sign = sign->next, row++) {
            data.push_back(sign);
        }
        fprintf(stderr, "Fill ency %d w. %d rows\n", (int)item_t, row);
        rows(row);
        return row;
    }

    void update(sign_rec_ptr first) {
        draw();
    }

    // Constructor
    //     Make our data array, and initialize the table options.
    //
    EncyTable(sign_rec_ptr first, EncyTable::item _item_t, 
        int X, int Y, int W, int H, const char* L = 0) : Fl_Table(X, Y, W, H, L) {
        int nrows;

        top = first;
        item_t = _item_t;
        if (first) {
            nrows = fill(first);
        }
        else {
            nrows = 0;
        }
        // Rows
        rows(nrows);             // how many rows
        row_header(0);              // enable row headers (along left)
        row_height_all(20);         // default height of rows
        row_resize(0);              // disable row resizing
        // Cols
        cols(MAX_COLS);             // how many columns
        col_header(1);              // enable column headers (along top)
        col_width_all(80);          // default width of columns
        col_resize(1);              // enable column resizing
        end();                      // end the Fl_Table group
    }
    ~EncyTable() { }
};

class EncyWin : public Fl_Double_Window {
public:
    EncyTable* table;

    EncyWin(sign_rec_ptr top, EncyTable::item _item_t) : Fl_Double_Window(900,400,"Encyclopedia") {
        table = new EncyTable(top, _item_t, 10, 10, 880, 380);
        end();
        resizable(table);
    }
    ~EncyWin() {
        delete table;
    }
};