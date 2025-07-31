
#include <string>
#include <vector>
#include <algorithm>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>

#include "agenda.h"
#include "hello.h"

class EncyTable : public Fl_Table {
    enum item;
    item item_t;
    std::vector<sign_rec_ptr> data;


    // Draw the row/col headings
    //    Make this a dark thin upbox with the text inside.
    //
    void DrawHeader(const char* s, int X, int Y, int W, int H);
    // Draw the cell data
    //    Dark gray text on white background with subtle border
    //
    void DrawData(const char* s, int X, int Y, int W, int H);
    // Handle drawing table's cells
    //     Fl_Table calls this function to draw each visible cell in the table.
    //     It's up to us to use FLTK's drawing functions to draw the cells the way we want.
    //
    void draw_cell(TableContext context, int ROW = 0, int COL = 0,
        int X = 0, int Y = 0, int W = 0, int H = 0) FL_OVERRIDE;

public:
    enum item {
        SIGN,
        HYPO,
        RULE,
    };
    sign_rec_ptr top;

    void clear();
    int fill(sign_rec_ptr first);
    void update(sign_rec_ptr first);

    EncyTable(sign_rec_ptr first, EncyTable::item _item_t,
        int X, int Y, int W, int H, const char* L);
    ~EncyTable();
};

class EncyWin : public Fl_Double_Window {
public:
    EncyTable* table;

    EncyWin(sign_rec_ptr top, EncyTable::item _item_t);
    ~EncyWin();
};