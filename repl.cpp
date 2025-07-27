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
void  repl_log( const char *s ){
  char buf[80];
  sprintf( buf, "%s\n", s );
  S_main_dlg->log( buf );
}

std::string local_val_repr( struct val_rec *val ){
  if( _UNKNOWN == val->status ) return std::string( "UNKNOWN" );
  switch( val->type ){
  case _VAL_T_BOOL:
    return( _FALSE == val->val_bool) ?
      std::string( "FALSE" ) : std::string( "TRUE" );
    break;
  case _VAL_T_INT:
    return( std::to_string( val->val_int ) );
    break;
  case _VAL_T_FLOAT:
     break;
  case _VAL_T_STR:
    if( val->valptr )
      return( std::string( val->valptr ) );
    else
      return( std::string( "error" ) );
    break;
  }
  return std::string( "error" );
}

int repl_color_code( FString str ){
  int res = 255;
  sign_rec_ptr sign = sign_find( str.toString().c_str(), (sign_rec_ptr) loadkb_get_allhypos() );

  if( sign &&
      ( _KNOWN == sign->val.status ) &&
      ( _VAL_T_BOOL == sign->val.type ) ){
    switch( sign->val.val_bool ){
    case _TRUE:
      res = 1;
      break;
    case _FALSE:
      res = 0;
      break;
    }
  }
  return res;
}

// std::string local_val_repr( const sign_rec_ptr s, struct val_rec *val ){
//   if( _UNKNOWN == val->status ) return std::string( "UNKNOWN" );
//   switch(s->len_type & TYPE_MASK){
//   case SIGN_MASK:
//     return( std::to_string( val->val_int ) );
//     break;
//   case COMPOUND_MASK:
//   case RULE_MASK:
//   case HYPO_MASK:
//     return( 0 == val->val_bool ? std::string( "FALSE" ) : std::string( "TRUE" ) );
//     break;
//   }
//   return std::string( "error" );
// }

//----------------------------------------------------------------------
// Getter callbacks from the engine
//----------------------------------------------------------------------
void getter_sign( sign_rec_ptr sign, int *suspend ){
  if(TRACE_ON) printf ("__FUNCTION__ = %s\n", __FUNCTION__);
  _UPDATE_ENCYS;
  if( S_main_dlg->q ) delete S_main_dlg->q;
  
  S_main_dlg->q = new QuestionWidget( S_main_dlg );
  S_main_dlg->q->current_sign = finalcut::FString( sign->str );
  S_main_dlg->q->input.setText( FString("Type answer here") );
  S_main_dlg->q->show();
  S_main_dlg->q->activateDialog();
  *suspend = _TRUE;
  // sign_set_default( sign, sign_get_default( sign ) );
}

// void fixCR( char *str ){
//   char *s = str;
//   while( *s ){
//     if( ('\n' == *s) && (0 != *(s+1)) ){
//       *(s+1) = 0;
//       return;
//     }
//     s++;
//   }
// }

static  struct val_rec v_true  = { _KNOWN, _VAL_T_BOOL, (char *)0, _TRUE, 0, 0.0, 0 };
static  struct val_rec v_false = { _KNOWN, _VAL_T_BOOL, (char *)0, _FALSE, 0, 0.0, 0 };

void engine_dsl_getter_compound( compound_rec_ptr compound, int *suspend ){
#ifdef ENGINE_DSL_HOWERJFORTH
  if( _KNOWN == compound->val.status ) return;
  
  int  err;
  char buf[128];
  sprintf( buf, "Getter compound %s (%d)\n", compound->str,
	   // (char *) (compound->dsl_expression)
	   *suspend
	   );
  repl_log( buf );
  // printf( buf );
  // WHY?
  // fixCR( compound->dsl_expression );
  int r = engine_dsl_eval_async( (const char *) compound->dsl_expression, &err, suspend );

  sprintf( buf, "FORTH Res %d Err %d Susp %d\n", r, err, *suspend );
  repl_log( buf );
  // printf( buf );
  sprintf( buf, "Post-eval compound %s (%d)\n", compound->str,
	   // (char *) (compound->dsl_expression)
	   *suspend
	   );
  repl_log( buf );
  switch( err ){
  case 0:
    // Ignore DSL evaluation if a question is pending! Re-evaluation will happen later.
    if( _FALSE == *suspend ){
      // sprintf( buf, "Getter compound %s (%d)\n", compound->str,
      // 	       // (char *) (compound->dsl_expression)
      // 	       *suspend
      // 	       );
      // printf( buf );
      sign_set_default( (sign_rec_ptr)compound, r ? &v_true : &v_false );
      // sprintf( buf, "Compound Status %d Type %d\n", compound->val.status, compound->val.type );
      // printf( "%s", buf );
    }
    break;
  }
  
#endif  
}

void cb_on_gate( sign_rec_ptr sign, short val ){
  char buf[64];
  sprintf( buf, "Gating %s (%d) - %d", sign->str, sign->val.val_bool, val );
  repl_log( buf );
  //
  engine_default_on_gate( sign, val );
  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate();
}

void cb_on_agenda_push( sign_rec_ptr sign, struct val_rec *val ){
  char buf[64];
  sprintf( buf, "Push %s", sign->str );
  repl_log( buf );
  //
  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate();
  engine_default_on_agenda_push( sign, val );
}

void cb_on_agenda_pop( sign_rec_ptr sign, struct val_rec *val ){
  char buf[64];
  sprintf( buf, "Pop %s", sign->str );
  repl_log( buf );

  S_main_dlg->EncyWindow[ENCY_AGND].ency->repopulate();
  // S_main_dlg->Trace.scrolltext.clear();
  engine_default_on_agenda_pop( sign, val );
}

void cb_on_set( sign_rec_ptr sign, struct val_rec *val ){
  char buf[128];
  std::string valstr_old = local_val_repr( &sign->val );
  std::string valstr_new = local_val_repr( val );
  sprintf( buf, "Set %s from %s to %s.", sign->str, valstr_old.c_str(), valstr_new.c_str() );
  repl_log( buf );

  _UPDATE_ENCYS;   
}

void cb_on_endsession( sign_rec_ptr sign, struct val_rec *val ){
  char buf[]="End of session.";
  repl_log( buf );
  _UPDATE_ENCYS;   
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
			   &cb_on_gate,
			   &cb_on_agenda_push,
			   &cb_on_agenda_pop,
			   &cb_on_endsession
			   );

  // Set up DSL
#ifdef ENGINE_DSL
  engine_dsl_init();
#endif

  // Create the FINALCUT application object
  finalcut::FApplication app(argc, argv);

    // Create main dialog object
  Menu main_dlg {&app};
  main_dlg.setText ("NXP");
  main_dlg.setSize ({40, 14});
  main_dlg.setShadow();
  S_main_dlg = &main_dlg;

  // Create main dialog object
  main_dlg.EncyWindow[ENCY_SIGN].ency = new Listview(&main_dlg, ENCY_SIGN, "Sign");
  main_dlg.EncyWindow[ENCY_SIGN].ency->setText (L"Encyclopedia: Signs");
  finalcut::FPoint position{25, 5};
  finalcut::FSize size{50, 20};
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


