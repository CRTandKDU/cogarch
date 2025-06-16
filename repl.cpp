/**
 * repl.cpp -- Simple FINALCUT-based GUI REPL
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
#include "listview.hpp"
#include "Menu.hpp"

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
//                               main part
//----------------------------------------------------------------------


auto main (int argc, char* argv[]) -> int
{
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

  // Create the FINALCUT application object
  finalcut::FApplication app(argc, argv);

    // Create main dialog object
  Menu main_dlg {&app};
  main_dlg.setText ("Session");
  main_dlg.setSize ({40, 14});
  main_dlg.setShadow();

  // Create main dialog object
  main_dlg.ency_sign = new Listview(&main_dlg, 0, "Sign");
  main_dlg.ency_sign->setText (L"Encyclopedia: Signs");
  finalcut::FPoint position{25, 5};
  finalcut::FSize size{37, 20};
  main_dlg.ency_sign->setGeometry ( position, size );
  main_dlg.ency_sign->setShadow();

  main_dlg.ency_hypo = new Listview(&main_dlg, 1, "Hypo");
  main_dlg.ency_hypo->setText (L"Encyclopedia: Hypos");
  finalcut::FPoint position_h{27, 7};
  main_dlg.ency_hypo->setGeometry ( position_h, size );
  main_dlg.ency_hypo->setShadow();

  // Set dialog d as main widget
  finalcut::FWidget::setMainWidget( &main_dlg );

  // Show and start the application
  main_dlg.show();

  int res = app.exec();

#ifdef ENGINE_DSL
  engine_dsl_free();
#endif
  loadkb_reset();
  engine_free_state( S_State );

  return res;
}


