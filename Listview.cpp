/**
 * Listview.cpp -- Encyclopedia sortable list view
 *
 * Written on Monday, June 16, 2025
 */
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <final/final.h>

#include "agenda.h"
#include "Listview.hpp"

extern engine_state_rec_ptr repl_getState();

//----------------------------------------------------------------------
// GUI
//----------------------------------------------------------------------
using finalcut::FPoint;
using finalcut::FSize;

//----------------------------------------------------------------------
Listview::Listview (finalcut::FWidget* parent, unsigned short ency_t, const char *title)
  : finalcut::FDialog{parent}
{
  this->ency_t	= ency_t;
  this->title	= title;
  
  // Add columns to the view
  listview.addColumn (title);
  listview.addColumn (ENCY_AGND == ency_t ? "Cause" : "Value");

  // Set right alignment for the third, fourth, and fifth column
  listview.setColumnAlignment (2, finalcut::Align::Right);

  // Set the type of sorting
  listview.setColumnSortType (1, finalcut::SortType::Name);

  // Sort in ascending order by the 1st column
  listview.setColumnSort (1, finalcut::SortOrder::Ascending);
  // Sorting follows later automatically on insert().
  // Otherwise you could start the sorting directly with sort()

  // Allways show the sort indicator 
  listview.hideSortIndicator(ency_t == ENCY_AGND ? true : false);

  // Populate FListView with a list of items
  populate();

  // Set push button text
  columns.setText (L"&Columns");
  // quit.setText (L"&Quit");

  // Add some function callbacks
  // quit.addCallback
  //   (
  //    "clicked",
  //    finalcut::getFApplication(),
  //    &finalcut::FApplication::cb_exitApp,
  //    this
  //    );

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

finalcut::FString val_repr( const sign_rec_ptr s, unsigned short ency_t ){
  if( _UNKNOWN == s->val ) return finalcut::FString( "UNKNOWN" );
  switch( ency_t ){
  case ENCY_HYPO:
    return( 0 == s->val ? finalcut::FString( "FALSE" ) : finalcut::FString( "TRUE" ) );
    break;
  case ENCY_SIGN:
    if( COMPOUND_MASK == (s->len_type & TYPE_MASK) )
          return( 0 == s->val ? finalcut::FString( "FALSE" ) : finalcut::FString( "TRUE" ) );
    return( std::move( finalcut::FString() << s->val ) );
    break;
  }
  return finalcut::FString( "error" );
}

//----------------------------------------------------------------------
void Listview::populate()
{
  switch( ency_t ){
  case ENCY_SIGN:
  case ENCY_HYPO:
    {
    std::vector<std::array<finalcut::FString, 2>> encyc = {};
    sign_rec_ptr s	= (this->ency_t == 0) ? loadkb_get_allsigns()
      : (this->ency_t == 1) ? loadkb_get_allhypos() : nullptr ;
    int len		= loadkb_howmany( s );
    if(TRACE_ON) fprintf( stderr, "LoadKB %d signs\n", len );
    for( unsigned short i=0; i<len; i++ ){
      std::array<finalcut::FString, 2> arr;
      arr.at(0) = s->str;
      arr.at(1) = val_repr( s, ency_t );
      encyc.push_back( arr );
      s = s->next;
    }

    for (const auto& place : encyc)
      {
	const finalcut::FStringList line (place.cbegin(), place.cend());
	listview.insert (line);
      }
    if(TRACE_ON)  fprintf( stderr, "Populated %d signs\n", len );
    }
    break;
    //
  case ENCY_AGND:
    std::vector<std::array<std::string, 2>> encyc = {};
    cell_rec_ptr cell = repl_getState()->agenda;
    for( cell_rec_ptr c = cell; c; c = c->next ){
      std::array<std::string, 2> arr;
      arr.at(0) = c->sign_or_hypo->str;
      arr.at(1) = std::string("Goal");
      encyc.push_back( arr );
    }
    for (const auto& place : encyc){
      const finalcut::FStringList line (place.cbegin(), place.cend());
      listview.insert (line);
    }
    break;
  }
}

//----------------------------------------------------------------------
void Listview::repopulate()
{
  listview.clear();
  populate();
  redraw();
}

//----------------------------------------------------------------------
finalcut::FListViewItem* Listview::getCurrentItem(){
  return listview.getCurrentItem();
}

//----------------------------------------------------------------------
void Listview::initLayout()
{
  // Set FListView geometry
  listview.setGeometry(FPoint{2, 1}, FSize{33, 14});
  // Set columns button geometry
  columns.setGeometry(FPoint{2, 16}, FSize{11, 1});
  // Set quit button geometry
  // quit.setGeometry(FPoint{24, 16}, FSize{10, 1});
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

