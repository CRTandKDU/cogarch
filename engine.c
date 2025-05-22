/**
 * engine.c -- Forward and Backward Chaining
 *
 * Written on jeudi, 22 mai 2025.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

#define _TRUE    ((unsigned short)1)
#define _FALSE   ((unsigned short)0)

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
    hypo->val = _FALSE;
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
    rule->val = _TRUE;
    return _TRUE;
  }
}

void engine_forward_rule( rule_rec_ptr rule ){
  //Short-circuit OR
  hypo_rec_ptr hypo = (hypo_rec_ptr) rule->setters;
  unsigned short val = rule->val;
  if( _TRUE == val ){
    hypo->val = _TRUE;
  }
  if( _FALSE == val ){
    unsigned short hval = engine_sc_or( hypo );
  }
  // TODO: Recursive forward
}

void engine_forward_cond( rule_rec_ptr rule, cond_rec_ptr cond ){
  // Short-circuit AND
  unsigned short val = cond->val;
  if( _FALSE == val ){
    rule->val = _FALSE;
  }
  if( _TRUE == val ){
    // TODO: Trigger postponed backward chaining (aka gating)
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
      engine_forward_cond( rule, cond ); 
    }
  }
}
