#ifndef MENU_HPP
#define MENU_HPP
#include "agenda.h"
#include "listview.hpp"

#define _MI_OPEN "&Open..."
#define _MI_QUIT "&Quit"
//----------------------------------------------------------------------
// class Menu
//----------------------------------------------------------------------

class Menu final : public finalcut::FDialog
{
public:
  // Constructor
  explicit Menu (finalcut::FWidget* = nullptr);

  // Members
  Listview *ency_sign;
  Listview *ency_hypo;

private:
  struct FileMenu
  {
    explicit FileMenu (finalcut::FMenuBar& menubar)
      : File{"&File", &menubar}
    { }

    finalcut::FMenu      File{};
    // finalcut::FMenuItem  New{"&New", &File};
    finalcut::FMenuItem  Open{_MI_OPEN, &File};
    // finalcut::FMenuItem  Save{"&Save", &File};
    // finalcut::FMenuItem  SaveAs{"&Save as...", &File};
    // finalcut::FMenuItem  Close{"&Close", &File};
    finalcut::FMenuItem  Line1{&File};
    // finalcut::FMenuItem  Print{"&Print", &File};
    // finalcut::FMenuItem  Line2{&File};
    finalcut::FMenuItem  Quit{_MI_QUIT, &File};
  };

  struct EditMenu
  {
    explicit EditMenu (finalcut::FMenuBar& menubar)
      : Edit{"&Edit", &menubar}
    { }

    finalcut::FMenu      Edit{};
    // finalcut::FMenuItem  Undo{FKey::Ctrl_z, "&Undo", &Edit};
    // finalcut::FMenuItem  Redo{FKey::Ctrl_y, "&Redo", &Edit};
    // finalcut::FMenuItem  Line3{&Edit};
    // finalcut::FMenuItem  Cut{FKey::Ctrl_x, "Cu&t", &Edit};
    // finalcut::FMenuItem  Copy{FKey::Ctrl_c, "&Copy", &Edit};
    // finalcut::FMenuItem  Paste{FKey::Ctrl_v, "&Paste", &Edit};
    // finalcut::FMenuItem  Line4{&Edit};
    // finalcut::FMenuItem  Search{FKey::Ctrl_f, "&Search", &Edit};
    // finalcut::FMenuItem  Next{FKey::F3, "Search &next", &Edit};
    // finalcut::FMenuItem  Line5{&Edit};
    // finalcut::FMenuItem  SelectAll{FKey::Ctrl_a, "Select &all", &Edit};
  };

  struct ExpertMenu
  {
    explicit ExpertMenu (finalcut::FMenuBar& menubar)
      : Expert{"E&xpert", &menubar}
    { }

    finalcut::FMenu  Expert{};
    finalcut::FMenuItem  Suggest{"&Suggest", &Expert};
    finalcut::FMenuItem  Volunteer{"&Volunteer", &Expert};
    finalcut::FMenuItem  Line5{&Expert};      
    finalcut::FMenuItem  Knowcess{"&Knowcess", &Expert};
  };

  struct EncyclopediaMenu
  {
    explicit EncyclopediaMenu (finalcut::FMenuBar& menubar)
      : Encyclopedia{"E&ncyclopedia", &menubar}
    { }

    finalcut::FMenu  Encyclopedia{};
    finalcut::FMenuItem  Signs{"S&igns", &Encyclopedia};
    finalcut::FMenuItem  Hypotheses{"&Hypotheses", &Encyclopedia};
  };


  // Methods
  void configureFileMenuItems();
  void configureEditMenuItems();
  void configureExpertMenuItems();
  void configureEncyclopediaMenuItems();
  void defaultCallback (const finalcut::FMenuList*);
  void initLayout() override;
  void adjustSize() override;

  // Ancillaries
  void log( const char *msg );

  // Event handler
  void onClose (finalcut::FCloseEvent*) override;

  // Callback methods
  void cb_message (const finalcut::FMenuItem*);
  void cb_open    (const finalcut::FMenuItem*);

  void cb_loadkb  ();

  // Data members
  finalcut::FMenuBar    Menubar{this};
  FileMenu              file_menu{Menubar};
  EditMenu              edit_menu{Menubar};
  ExpertMenu            expert_menu{Menubar};
  EncyclopediaMenu      encyclopedia_menu{Menubar};

  finalcut::FMenuItem   Window{"&Window", &Menubar};
  finalcut::FMenuItem   Help{"&Help", &Menubar};
  finalcut::FStatusBar  Statusbar{this};
  finalcut::FLabel      Headline1{this};
  finalcut::FLabel      Headline2{this};
  finalcut::FLabel      Info{this};
  finalcut::FListView   Trace{this};
};
#endif
