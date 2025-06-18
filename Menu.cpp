/**
 * Menu.cpp -- Menu Bar
 *
 * Written on Monday, June 16, 2025
 */

#include <final/final.h>
#include "agenda.h"
#include "Menu.hpp"

#define _IF_MENUITEM(str)     if ( item \
      && item->isEnabled()              \
      && item->acceptFocus()            \
      && item->isVisible()              \
      && ! item->isSeparator()          \
      && item->getText() == (str) )


using FKey = finalcut::FKey;
using finalcut::FPoint;
using finalcut::FSize;

extern engine_state_rec_ptr repl_getState();
extern Menu *repl_getMainDlg();

//----------------------------------------------------------------------
Menu::Menu (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Menu bar itms
  file_menu.File.setStatusbarMessage ("File management commands");
  edit_menu.Edit.setStatusbarMessage ("Cut-and-paste editing commands");
  expert_menu.Expert.setStatusbarMessage ("Engine/Session commands");
  encyclopedia_menu.Encyclopedia.setStatusbarMessage ("Encyclopedia: Signs, Hypotheses, Rules");
  Window.setDisable();
  Help.setStatusbarMessage ("Show version and copyright information");

  // Menu items
  configureFileMenuItems();
  configureEditMenuItems();
  configureExpertMenuItems();
  configureEncyclopediaMenuItems();

  // Add default menu item callback
  defaultCallback (&Menubar);

  addCallback( "event_loadkb", this, &Menu::cb_loadkb );

  // Statusbar at the bottom
  Statusbar.setMessage("Status bar message");

  // Headline labels
  // Headline1 << " Key ";
  // Headline1.ignorePadding();
  // Headline1.setEmphasis();

  // Headline2 << " Function ";
  // Headline2.ignorePadding();
  // Headline2.setEmphasis();

  // Info label
  // Info << "<F10>            Activate menu bar\n"
  //      << "<Ctrl>+<Space>   Activate menu bar\n"
  //      << "<Menu>           Activate menu bar\n"
  //      << "<Shift>+<Menu>   Open dialog menu\n"
  //      << "<Meta>+<Q>       Exit";

  Trace.addColumn( L"Log" );
  Trace.hideSortIndicator(true);
  log( "Session inited." );
}

//----------------------------------------------------------------------
void Menu::configureFileMenuItems()
{
  // "File" menu items
  // file_menu.New.addAccelerator (FKey::Ctrl_n);  // Ctrl + N
  // file_menu.New.setStatusbarMessage ("Create a new file");
  file_menu.Open.addAccelerator (FKey::Ctrl_o);  // Ctrl + O
  file_menu.Open.setStatusbarMessage ("Locate and open a knowledge base file");
  // file_menu.Save.addAccelerator (FKey::Ctrl_s);  // Ctrl + S
  // file_menu.Save.setStatusbarMessage ("Save the file");
  // file_menu.SaveAs.setStatusbarMessage ("Save the current file under a different name");
  // file_menu.Close.addAccelerator (FKey::Ctrl_w);  // Ctrl + W
  // file_menu.Close.setStatusbarMessage ("Close the current file");
  file_menu.Line1.setSeparator();
  // file_menu.Print.addAccelerator (FKey::Ctrl_p);  // Ctrl + P
  // file_menu.Print.setStatusbarMessage ("Print the current file");
  // file_menu.Line2.setSeparator();
  file_menu.Quit.addAccelerator (FKey::Meta_q);  // Meta/Alt + Q
  file_menu.Quit.setStatusbarMessage ("Exit the program");

  // Add quit menu item callback
  file_menu.Quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void Menu::configureEditMenuItems()
{
  // "Edit" menu items
  // edit_menu.Undo.setStatusbarMessage ("Undo the previous operation");
  // edit_menu.Redo.setDisable();
  // edit_menu.Line3.setSeparator();
  // edit_menu.Cut.setStatusbarMessage ("Remove the input text "
  //                                    "and put it in the clipboard");
  // edit_menu.Copy.setStatusbarMessage ("Copy the input text into the clipboad");
  // edit_menu.Paste.setStatusbarMessage ("Insert text form clipboard");
  // edit_menu.Line4.setSeparator();
  // edit_menu.Search.setStatusbarMessage ("Search for text");
  // edit_menu.Next.setStatusbarMessage ("Repeat the last search command");
  // edit_menu.Line5.setSeparator();
  // edit_menu.SelectAll.setStatusbarMessage ("Select the whole text");
}

//----------------------------------------------------------------------
void Menu::configureExpertMenuItems()
{
  // "Expert" menu items
  expert_menu.Suggest.setStatusbarMessage ("Suggest hypothesis");
  expert_menu.Suggest.addAccelerator (FKey::Ctrl_s);
  expert_menu.Volunteer.setStatusbarMessage ("Volunteer sign's value");
  expert_menu.Volunteer.addAccelerator (FKey::Ctrl_v);
  expert_menu.Volunteer.setStatusbarMessage ("Clears all values to UNKNOWN");
  expert_menu.Volunteer.addAccelerator (FKey::Ctrl_r);
  expert_menu.Line5.setSeparator();
  expert_menu.Agenda.setStatusbarMessage ("Show/Hide Agenda");
  expert_menu.Agenda.addAccelerator (FKey::Ctrl_a);
  expert_menu.Knowcess.setStatusbarMessage ("Run session");
  expert_menu.Knowcess.addAccelerator (FKey::Ctrl_k);
}

//----------------------------------------------------------------------
void Menu::configureEncyclopediaMenuItems()
{
  // "Encyclopedia" menu items
  encyclopedia_menu.Signs.setStatusbarMessage ("Show/Hide List of Signs");
  encyclopedia_menu.Signs.addAccelerator (FKey::Ctrl_d);
  encyclopedia_menu.Hypotheses.setStatusbarMessage ("Show/Hide List of Hypotheses");
  encyclopedia_menu.Hypotheses.addAccelerator (FKey::Ctrl_y);
}

//----------------------------------------------------------------------
void Menu::cb_loadkb()
{
  EncyWindow[ENCY_SIGN].ency->repopulate();
  EncyWindow[ENCY_HYPO].ency->repopulate();
  Trace.redraw();
}

//----------------------------------------------------------------------
void Menu::defaultCallback (const finalcut::FMenuList* mb)
{
  for (std::size_t i{1}; i <= mb->getCount(); i++)
  {
    auto item = mb->getItem(int(i));

    _IF_MENUITEM(_MI_OPEN){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_open,
        item
      );
      continue;
    }
    _IF_MENUITEM(_MI_KNOWCESS){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_knowcess,
	item
      );
      continue;
    }
    _IF_MENUITEM(_MI_SUGGEST){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_suggest,
	item
      );
      continue;
    }
    _IF_MENUITEM(_MI_AGENDA){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_ency,
	item
      );
      continue;
    }
    _IF_MENUITEM(_MI_SIGNS){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_ency,
	item
      );
      continue;
    }
    _IF_MENUITEM(_MI_HYPOS){
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_ency,
	item
      );
      continue;
    }

    if ( item
      && item->isEnabled()
      && item->acceptFocus()
      && item->isVisible()
      && ! item->isSeparator()
      && item->getText() != _MI_QUIT )
    {
      // Add the callback function
      item->addCallback
      (
        "clicked",
        this, &Menu::cb_message,
        item
      );

      // Call sub-menu
      if ( item->hasMenu() )
        defaultCallback (item->getMenu());
    }
  }
}

//----------------------------------------------------------------------
void Menu::initLayout()
{
  // Headline1.setGeometry (FPoint{ 3, 2}, FSize{ 5, 1});
  // Headline2.setGeometry (FPoint{19, 2}, FSize{10, 1});
  // Info.setGeometry(FPoint{2, 1}, FSize{36, 5});
  Trace.setGeometry( FPoint{2,2}, FSize{36,10} );
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Menu::adjustSize()
{
  const auto pw = int(getDesktopWidth());
  const auto ph = int(getDesktopHeight());
  setX (1 + (pw - int(getWidth())) / 2, false);
  setY (1 + (ph - int(getHeight())) / 4, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Menu::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Menu::cb_message (const finalcut::FMenuItem* menuitem)
{
  auto text = menuitem->getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::info ( this
                              , "Info"
                              , "You have chosen \"" + text + "\"" );
}

//----------------------------------------------------------------------
void Menu::cb_open (const finalcut::FMenuItem* menuitem)
{
  // Load knowledge base using the File Picker
  finalcut::FString         directory{L"."};
  finalcut::FString         filter("*.org");
  finalcut::FString         filename = 
    finalcut::FFileDialog::fileOpenChooser(this, directory, filter);

  if ( filename.isEmpty() )
    return;

  int res = loadkb_file( filename.c_str() );
  if(TRACE_ON) fprintf( stderr, "Result %d\n", res );
  if( res ) exit(res);

  char buf[64];
  sprintf( buf, "Loaded KB %s.", filename.c_str() );
  log( buf );
  emitCallback("event_loadkb");
}

//----------------------------------------------------------------------
void Menu::cb_ency (const finalcut::FMenuItem* menuitem)
{
  short idx = -1;
  auto text = menuitem->getText();
  if( text == _MI_SIGNS )	idx = ENCY_SIGN;
  if( text == _MI_HYPOS )	idx = ENCY_HYPO;
  if( text == _MI_AGENDA )      idx = ENCY_AGND;
  if( -1 == idx ) return;

  if( EncyWindow[idx].ency_visible ){
    EncyWindow[idx].ency->hide();
  }
  else{
    EncyWindow[idx].ency->show();
    EncyWindow[idx].ency->redraw();
    EncyWindow[idx].ency->activateDialog();
  }
  EncyWindow[idx].ency_visible = 1 - EncyWindow[idx].ency_visible;
}

//----------------------------------------------------------------------
void Menu::cb_suggest (const finalcut::FMenuItem* menuitem){
  finalcut::FListViewItem *hypo = EncyWindow[ENCY_HYPO].ency->getCurrentItem();
  auto text = hypo->getText( 1 );

  hypo_rec_ptr h = (hypo_rec_ptr)sign_find( text.c_str(), loadkb_get_allhypos() );
  if( h ){
    const auto& ret =				\
      finalcut::FMessageBox::info ( this, "Confirm suggestion"
			  , "Do you really want\n"
                          "to suggest\n" + text
			  , finalcut::FMessageBox::ButtonType::Yes
			  , finalcut::FMessageBox::ButtonType::No );
    if ( ret == finalcut::FMessageBox::ButtonType::Yes ){
      engine_pushnew_hypo( repl_getState(), h );
      char buf[64];
      sprintf( buf, "Suggested %s.", h->str );
      log( buf );
    }
  }
  
}

//----------------------------------------------------------------------
void Menu::cb_knowcess (const finalcut::FMenuItem* menuitem){
  engine_resume_knowcess( repl_getState() );
  repl_getMainDlg()->EncyWindow[ENCY_SIGN].ency->repopulate();
  repl_getMainDlg()->EncyWindow[ENCY_HYPO].ency->repopulate();
  repl_getMainDlg()->EncyWindow[ENCY_AGND].ency->repopulate();
}

//----------------------------------------------------------------------
void Menu::log( const char *msg ){
  std::vector<std::array<std::string, 1>> lines = {};
  std::array<std::string, 1> arr;
  arr.at(0) = std::string( msg );
  lines.push_back( arr );
  for (const auto& place : lines)
    {
      const finalcut::FStringList line (place.cbegin(), place.cend());
      Trace.insert (line);
    }

}



//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

// auto main (int argc, char* argv[]) -> int
// {
//   // Create the application object
//   finalcut::FApplication app {argc, argv};

//   // Create main dialog object
//   Menu main_dlg {&app};
//   main_dlg.setText ("Menu example");
//   main_dlg.setSize ({40, 8});
//   main_dlg.setShadow();

//   // Set dialog main_dlg as main widget
//   finalcut::FWidget::setMainWidget (&main_dlg);

//   // Show and start the application
//   main_dlg.show();
//   return app.exec();
// }
