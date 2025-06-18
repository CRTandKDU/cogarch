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
#include "Listview.hpp"
#include "Menu.hpp"
#include "Question.hpp"

#define _UPDATE_ENCYS   S_main_dlg->EncyWindow[ENCY_SIGN].ency->repopulate(); \
  S_main_dlg->EncyWindow[ENCY_HYPO].ency->repopulate(); \
  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate(); \


//----------------------------------------------------------------------
// Minimal setup and ancillaries for engine
//----------------------------------------------------------------------
engine_state_rec_ptr S_State;
engine_state_rec_ptr repl_getState(){ return S_State; }

Menu *S_main_dlg = nullptr;
Menu* repl_getMainDlg(){ return S_main_dlg; }

std::string local_val_repr( const sign_rec_ptr s, int val ){
  if( _UNKNOWN == val ) return std::string( "UNKNOWN" );
  switch(s->len_type & TYPE_MASK){
  case SIGN_MASK:
    return( std::to_string( val ) );
    break;
  case COMPOUND_MASK:
  case RULE_MASK:
  case HYPO_MASK:
    return( 0 == val ? std::string( "FALSE" ) : std::string( "TRUE" ) );
    break;
  }
  return std::string( "error" );
}

//----------------------------------------------------------------------
// Getter callbacks from the engine
//----------------------------------------------------------------------
void getter_sign( sign_rec_ptr sign ){
  if(TRACE_ON) printf ("__FUNCTION__ = %s\n", __FUNCTION__);
  _UPDATE_ENCYS;
  
  S_main_dlg->q->current_sign = finalcut::FString( sign->str );
  S_main_dlg->q->input.setText( FString("Type answer here") );
  S_main_dlg->q->redraw();
  S_main_dlg->q->show();
  S_main_dlg->q->activateDialog();
  // sign_set_default( sign, sign_get_default( sign ) );
}

void engine_dsl_getter_compound( compound_rec_ptr compound ){
#ifdef ENGINE_DSL_HOWERJFORTH
  int r;
  if(TRACE_ON) printf("<FORTH> Compound %s\n%s\n", compound->str, (char *)compound->dsl_expression );
  S_main_dlg->q->setModal( true );
  r = engine_dsl_eval( (char *) (compound->dsl_expression) );
  /* if( 65535 == r ) r = _TRUE; // -1 is true in FORTH */
  if(TRACE_ON) printf("<FORTH> Evaluated to %d\n", r );
  S_main_dlg->q->setModal(false);
  sign_set_default( (sign_rec_ptr)compound, r );
#endif  
}

void cb_on_agenda_push( sign_rec_ptr sign, short val ){
  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate();
  engine_default_on_agenda_push( sign, val );
}

void cb_on_agenda_pop( sign_rec_ptr sign,  short val ){
  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate();
  engine_default_on_agenda_pop( sign, val );
}

void cb_on_set( sign_rec_ptr sign, short val ){
  char buf[64];
  sprintf( buf, "Set %s = %s.", sign->str, local_val_repr(sign, (int)val ).c_str() );
  S_main_dlg->log( buf );
  S_main_dlg->redraw();
  // Important! This is where sign's values are forwarded.
  engine_default_on_set( sign, val );
}

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
			   &cb_on_set,
			   &engine_default_on_gate,
			   &cb_on_agenda_push,
			   &cb_on_agenda_pop
			   );

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
  S_main_dlg = &main_dlg;

  // Create main dialog object
  main_dlg.EncyWindow[ENCY_SIGN].ency = new Listview(&main_dlg, ENCY_SIGN, "Sign");
  main_dlg.EncyWindow[ENCY_SIGN].ency->setText (L"Encyclopedia: Signs");
  finalcut::FPoint position{25, 5};
  finalcut::FSize size{37, 20};
  main_dlg.EncyWindow[ENCY_SIGN].ency->setGeometry ( position, size );
  main_dlg.EncyWindow[ENCY_SIGN].ency->setShadow();
  main_dlg.EncyWindow[ENCY_SIGN].ency->hide();

  main_dlg.EncyWindow[ENCY_HYPO].ency = new Listview(&main_dlg, ENCY_HYPO, "Hypo");
  main_dlg.EncyWindow[ENCY_HYPO].ency->setText (L"Encyclopedia: Hypos");
  finalcut::FPoint position_h{27, 7};
  main_dlg.EncyWindow[ENCY_HYPO].ency->setGeometry ( position_h, size );
  main_dlg.EncyWindow[ENCY_HYPO].ency->setShadow();
  main_dlg.EncyWindow[ENCY_HYPO].ency->hide();

  main_dlg.EncyWindow[ENCY_AGND].ency = new Listview(&main_dlg, ENCY_AGND, "Goal");
  main_dlg.EncyWindow[ENCY_AGND].ency->setText (L"Agenda");
  finalcut::FPoint position_a{29, 9};
  main_dlg.EncyWindow[ENCY_AGND].ency->setGeometry ( position_a, size );
  main_dlg.EncyWindow[ENCY_AGND].ency->setShadow();
  main_dlg.EncyWindow[ENCY_AGND].ency->hide();

  main_dlg.q = new QuestionWidget( &main_dlg );
  main_dlg.q->current_sign = finalcut::FString( "" );
  main_dlg.q->hide();

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


