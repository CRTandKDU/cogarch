#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <final/final.h>

#include "agenda.h"

//----------------------------------------------------------------------
// Minimal setup and ancillaries for engine
//----------------------------------------------------------------------
engine_state_rec_ptr S_State;

const char *S_Color[] = { "\x1b[38;5;46m", "\x1b[38;5;160m", "\x1b[38;5;15m" };

char *S_val_color( unsigned short val ){
  char *esc;
  switch( val ){
  case _TRUE:
    esc = (char *) S_Color[0];
    break;
  case _FALSE:
    esc = (char *) S_Color[1];
    break;
  default:
    esc = (char *) S_Color[2];
  }
  return esc;
}

//----------------------------------------------------------------------
// GUI
//----------------------------------------------------------------------
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Listview
//----------------------------------------------------------------------

class Listview final : public finalcut::FDialog
{
public:
  // Constructor
  explicit Listview (finalcut::FWidget* = nullptr, sign_rec_ptr top = nullptr, const char *title = "");
  sign_rec_ptr top;
  const char *title;

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
  finalcut::FButton   quit{this};

};

//----------------------------------------------------------------------
Listview::Listview (finalcut::FWidget* parent, sign_rec_ptr top, const char *title)
  : finalcut::FDialog{parent}
{
  this->top	= top;
  this->title	= title;
  
  // Add columns to the view
  listview.addColumn (title);
  listview.addColumn ("Value");

  // Set right alignment for the third, fourth, and fifth column
  listview.setColumnAlignment (2, finalcut::Align::Right);

  // Set the type of sorting
  listview.setColumnSortType (1, finalcut::SortType::Name);

  // Sort in ascending order by the 1st column
  listview.setColumnSort (1, finalcut::SortOrder::Ascending);
  // Sorting follows later automatically on insert().
  // Otherwise you could start the sorting directly with sort()

  // Allways show the sort indicator (▼/▲)
  listview.hideSortIndicator(false);

  // Populate FListView with a list of items
  populate();

  // Set push button text
  columns.setText (L"&Columns");
  quit.setText (L"&Quit");

  // Add some function callbacks
  quit.addCallback
    (
     "clicked",
     finalcut::getFApplication(),
     &finalcut::FApplication::cb_exitApp,
     this
     );

  columns.addCallback
    (
     "clicked",
     this, &Listview::cb_showHideColumns
     );

  listview.addCallback
    (
     "clicked",
     this, &Listview::cb_showInMessagebox
     );

  if(TRACE_ON) fprintf( stderr, "Init done!\n" );

}

//----------------------------------------------------------------------
void Listview::populate()
{
  std::vector<std::array<std::string, 2>> encyc = {};
  sign_rec_ptr s	= this->top;
  int len		= loadkb_howmany( s );
  if(TRACE_ON) fprintf( stderr, "LoadKB %d signs\n", len );
  for( unsigned short i=0; i<len; i++ ){
    std::array<std::string, 2> arr;
    arr.at(0) = s->str;
    arr.at(1) = 255 == s->val ? std::string("unknown") : ( 1 == s->val ? "true" : "false" );
    encyc.push_back( arr );
    s = s->next;
  }

  for (const auto& place : encyc)
    {
      const finalcut::FStringList line (place.cbegin(), place.cend());
      listview.insert (line);
    }
  if(TRACE_ON) fprintf( stderr, "Populated %d signs\n", len );

}

//----------------------------------------------------------------------
void Listview::initLayout()
{
  // Set FListView geometry
  listview.setGeometry(FPoint{2, 1}, FSize{33, 14});
  // Set columns button geometry
  columns.setGeometry(FPoint{2, 16}, FSize{11, 1});
  // Set quit button geometry
  quit.setGeometry(FPoint{24, 16}, FSize{10, 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Listview::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Listview::cb_showInMessagebox()
{
  const auto& item = listview.getCurrentItem();
  finalcut::FMessageBox info ( "Sign " + item->getText(1)
			       , "  Value: " + item->getText(2) + "\n"
			       , finalcut::FMessageBox::ButtonType::Ok
			       , finalcut::FMessageBox::ButtonType::Reject
			       , finalcut::FMessageBox::ButtonType::Reject
			       , this );
  info.show();
}

//----------------------------------------------------------------------
void Listview::cb_showHideColumns()
{
  finalcut::FMessageBox column_header_dlg \
    (
     "Show columns"
     , "\n\n\n\n"
     , finalcut::FMessageBox::ButtonType::Ok
     , finalcut::FMessageBox::ButtonType::Cancel
     , finalcut::FMessageBox::ButtonType::Reject
     , this
     );

  auto number_of_columns = listview.getColumnCount();
  std::vector<std::shared_ptr<finalcut::FCheckBox>> checkboxes{};

  for (std::size_t column{0}; column < number_of_columns; column++)
    {
      auto col_name = listview.getColumnText(int(column) + 1);
      checkboxes.emplace_back(std::make_shared<finalcut::FCheckBox>(col_name, &column_header_dlg));
      checkboxes[column]->setGeometry (FPoint{6, 4 + int(column)}, FSize{20, 1});

      if ( ! listview.isColumnHidden(int(column) + 1) )
	checkboxes[column]->setChecked();
    }

  column_header_dlg.setHeadline("Select columns to view");
  const auto& ret = column_header_dlg.exec();

  if ( ret != finalcut::FMessageBox::ButtonType::Ok )
    return;

  for (std::size_t column{0}; column < number_of_columns; column++)
    {
      if ( listview.isColumnHidden(int(column) + 1) && checkboxes[column]->isChecked() )
	listview.showColumn(int(column) + 1);
      else if ( ! listview.isColumnHidden(int(column) + 1) && ! checkboxes[column]->isChecked() )
	listview.hideColumn(int(column) + 1);
    }
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------


auto main (int argc, char* argv[]) -> int
{
  // Create demo knowledge base
  // New state
  S_State		= (engine_state_rec_ptr)malloc( sizeof( struct engine_state_rec ) );
  S_State->current_sign = (sign_rec_ptr)0;
  S_State->agenda	= (cell_rec_ptr)0;
  engine_register_effects( &engine_default_on_get,
			   &engine_default_on_set,
			   &engine_default_on_gate);

  // Set up DSL
#ifdef ENGINE_DSL
  engine_dsl_init();
#endif

  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Load knowledge base from the File Picker
  finalcut::FString         directory{L"."};
  finalcut::FString         filter("*.org");
  finalcut::FString         filename = 
    finalcut::FFileDialog::fileOpenChooser(&app, directory, filter);

  if ( filename.isEmpty() )
    return 1;

  int res = loadkb_file( filename.c_str() );
  if(TRACE_ON) fprintf( stderr, "Result %d\n", res );
  if( res ) return res;
  
  // Create main dialog object
  Listview d_sign(&app, loadkb_get_allsigns(), "Sign");
  d_sign.setText (L"Encyclopedia: Signs");
  finalcut::FPoint position{25, 5};
  finalcut::FSize size{37, 20};
  d_sign.setGeometry ( position, size );
  d_sign.setShadow();

  Listview d_hypo(&d_sign, loadkb_get_allhypos(), "Hypo");
  d_hypo.setText (L"Encyclopedia: Hypos");
  d_hypo.setGeometry ( position, size );
  d_hypo.setShadow();

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget( &d_sign );

  // Show and start the application
  d_sign.show();

  return app.exec();
}
