/**
 * engine.c -- Forward and Backward Chaining
 *
 * Written on jeudi, 22 mai 2025.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

extern engine_state_rec_ptr S_State;
effect S_on_get		= (effect)0;  // Triggered on get in `sign_default_get`
effect S_on_set		= (effect)0;  // Triggered on set in `sign_default_set`
effect S_on_gate	= (effect)0;  // Triggered on setting cond in `engine_forward_sign`


void engine_default_on_get( sign_rec_ptr sign, unsigned short val ){
  // Do nothing
}

void engine_default_on_set( sign_rec_ptr sign, unsigned short val ){
  // Forward signs and hypos
  switch( sign->len_type & TYPE_MASK ){
  case SIGN_MASK:
  case COMPOUND_MASK:
  case HYPO_MASK:
    engine_forward_sign( sign );
    break;
  }
}

void engine_default_on_gate( hypo_rec_ptr hypo, unsigned short val ){
  // Postpone hypo when one of its rule might be true
  if( _TRUE == val && _UNKNOWN == hypo->val ){
    cell_rec_ptr new_cell, cell = S_State->agenda;
    if( cell ){
      while( cell->next ) cell = cell->next;
      new_cell			= (cell_rec_ptr)malloc( sizeof( struct cell_rec ) );
      new_cell->sign_or_hypo	= hypo;
      new_cell->val		= _UNKNOWN;
      new_cell->next		= cell->next;
      cell->next		= new_cell;
    }
    else{
      engine_pushnew_hypo( S_State, hypo );
    }
  }
}

void engine_register_effects( effect f_get, effect f_set, effect f_gate ){
  S_on_get	= f_get;
  S_on_set	= f_set;
  S_on_gate	= f_gate;
}

void            engine_free_state( engine_state_rec_ptr state ){
  cell_rec_ptr prev, cell = state->agenda;
  while( cell ){
    prev = cell;
    cell = prev->next;
    free( (void *)prev );
  }
  free( (void *)state );
}

void engine_print_state( engine_state_rec_ptr state ){
  cell_rec_ptr prev, cell = state->agenda;
  printf( "STATE> Current sign: %s\n", (state->current_sign ? state->current_sign->str : "NONE") );
  while( cell ){
    prev = cell;
    cell = prev->next;
    printf( "STATE>\t%s\t%s\t%s\n",
	    _UNKNOWN == prev->val ? "SUGGEST" : "VOLUNTEER",
	    prev->sign_or_hypo->str,
	    _UNKNOWN == prev->val ? "" : ( _TRUE == prev->val ? "TRUE" : "FALSE" ));
  }
  printf( "----\t----\t----\t----\n" );
}

//
void            engine_pushnew_hypo( engine_state_rec_ptr state, hypo_rec_ptr h ){
  // Suggest
  cell_rec_ptr cell = (cell_rec_ptr)malloc( sizeof( struct cell_rec ) );
  cell->next = state->agenda;
  state->agenda = cell;
  cell->sign_or_hypo = h;
  cell->val = _UNKNOWN;
}

void engine_pushnew_signdata( engine_state_rec_ptr state, sign_rec_ptr sign, unsigned short val ){
  // Volunteer
  cell_rec_ptr cell = (cell_rec_ptr)malloc( sizeof( struct cell_rec ) );
  cell->next = state->agenda;
  state->agenda = cell;
  cell->sign_or_hypo = sign;
  cell->val = val;
}

void engine_knowcess( engine_state_rec_ptr state ){
  // Execute `suggest` and `volunteer` commands until quiescent state
  while( state->agenda ){
    cell_rec_ptr cell = state->agenda;
    // Test for `suggest hypo` or compound
    if( _UNKNOWN == cell->val ){
      switch( cell->sign_or_hypo->len_type & TYPE_MASK ){
      case COMPOUND_MASK:
	engine_backward_compound( (compound_rec_ptr) cell->sign_or_hypo );
	break;
      case HYPO_MASK:
	engine_backward_hypo( (hypo_rec_ptr) cell->sign_or_hypo );
	break;
      }
    }
    else{
      sign_set_default( (sign_rec_ptr) cell->sign_or_hypo, cell->val );
    }
    // Now pop from agenda
    state->agenda = cell->next;
  }
}

unsigned short engine_sc_or( hypo_rec_ptr hypo ){
  switch( hypo->val ){
  case _TRUE:
  case _FALSE:
    return _TRUE;
    break;
  default:
    bwrd_rec_ptr bwrd;
    for( unsigned short i=0; i<hypo->ngetters; i++ ){
      bwrd = (bwrd_rec_ptr) (hypo->getters)[i];
      if( _FALSE != bwrd->rule->val ){
	return _FALSE;
      }
    }
    /* hypo->val = _FALSE; */
    sign_set_default( hypo, _FALSE );
    return _TRUE;
  }
}

unsigned short engine_sc_and( rule_rec_ptr rule ){
  switch( rule->val ){
  case _TRUE:
  case _FALSE:
    return _TRUE;
    break;
  default:
    cond_rec_ptr cond;
    for( unsigned short i=0; i<rule->ngetters; i++ ){
      cond = (cond_rec_ptr) (rule->getters)[i];
      if( _TRUE != cond->val ){
	return _FALSE;
      }
    }
    /* rule->val = _TRUE; */
    sign_set_default( rule, _TRUE );
    return _TRUE;
  }
}

void engine_forward_rule( rule_rec_ptr rule ){
  // Short-circuit OR.
  // Recursive forward (on hypos) built-in the `sign-set-default` function
  hypo_rec_ptr hypo = (hypo_rec_ptr) rule->setters;
  unsigned short val = rule->val;
  if( _TRUE == val ){
    /* hypo->val = _TRUE; */
    sign_set_default( hypo, _TRUE );
  }
  if( _FALSE == val ){
    unsigned short hval = engine_sc_or( hypo );
  }
}

void engine_forward_cond( rule_rec_ptr rule, cond_rec_ptr cond ){
  // Short-circuit AND
  unsigned short val = cond->val;
  if( _FALSE == val ){
    /* rule->val = _FALSE; */
    sign_set_default( rule, _FALSE );
  }
  if( _TRUE == val ){
    unsigned short rval = engine_sc_and( rule );
  }
  engine_forward_rule( rule );
}

void engine_forward_sign( sign_rec_ptr sign ){
  unsigned short val = sign->val;
  if( sign->nsetters ){
    for( unsigned short i = 0; i<sign->nsetters; i++ ){
      fwrd_rec_ptr fwrd = (fwrd_rec_ptr) (sign->setters)[i];
      rule_rec_ptr rule = fwrd->rule;
      cond_rec_ptr cond = (cond_rec_ptr) (rule->getters)[ fwrd->idx_cond ];
      cond->val = (cond->out == val) ? _TRUE : _FALSE;
      // Gating on known condition
      if( S_on_gate ) S_on_gate( (hypo_rec_ptr) rule->setters, cond->val );
      engine_forward_cond( rule, cond ); 
    }
  }
}

void engine_backward_hypo( hypo_rec_ptr hypo ){
  if( _UNKNOWN != hypo->val ) return;
  // Sequential OR
  bwrd_rec_ptr bwrd;
  for( unsigned short i=0; i<hypo->ngetters; i++ ){
    bwrd = (bwrd_rec_ptr) (hypo->getters)[i];
    engine_backward_rule( bwrd->rule );
    // Check for side-effects of short-circuiting forward chaining
    if( _UNKNOWN != hypo->val ) break;
  }
}

void engine_backward_compound( compound_rec_ptr compound ){
  if( COMPOUND_MASK != (compound->len_type & TYPE_MASK) ) return;
  if( _UNKNOWN != compound->val ) return;
  if( 0 == compound->ngetters ){
    // Field getters is a pointer to the getter function
    ((sign_getter_t)(compound->getters))( (sign_rec_ptr)compound );
  }
}

void engine_backward_rule( rule_rec_ptr rule ){
  if( _UNKNOWN != rule->val ) return;
  // Sequential AND
  cond_rec_ptr cond;
  for( unsigned short i=0; i<rule->ngetters; i++ ){
    cond = (cond_rec_ptr) (rule->getters)[i];
    engine_backward_cond( cond );
    // Check for side-effects of short-circuiting forward chaining
    if( _UNKNOWN != rule->val ) break;
  }
}

void engine_backward_cond( cond_rec_ptr cond ){
  if( _UNKNOWN == cond->sign->val ){
    // Hypothesis: backward on rules
    if( HYPO_MASK == (cond->sign->len_type & TYPE_MASK) ){
      engine_backward_hypo( (hypo_rec_ptr) cond->sign );
    }
    // Sign or Compound: ask or execute DSL program
    else{
      if( 0 == cond->sign->ngetters ){
	// Field getters is a pointer to the getter function
	((sign_getter_t)(cond->sign->getters))( cond->sign );
      }
    }
  }
}

