#ifndef LISTVIEW_HPP
#define LISTVIEW_HPP
#include "agenda.h"

//----------------------------------------------------------------------
// class Listview
//----------------------------------------------------------------------

class Listview final : public finalcut::FDialog
{
public:
  // Constructor
  explicit Listview (finalcut::FWidget* = nullptr, unsigned short ency_t = 0, const char *title = "");

  // Members
  unsigned short ency_t;
  const char *title;

  // Methods
  void repopulate();
  finalcut::FListViewItem* getCurrentItem();

private:
  // Method
  void populate();
  void initLayout() override;

  // Event handlers
  void onClose (finalcut::FCloseEvent*) override;

  // Callback method
  void cb_showInMessagebox();
  void cb_showHideColumns();

  // Data members
  finalcut::FListView listview{this};
  finalcut::FButton   columns{this};
  // finalcut::FButton   quit{this};
};

#endif
