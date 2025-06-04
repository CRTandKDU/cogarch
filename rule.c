/**
 * rule.c -- Rules associate a set of conditions to a hypothesis
 *
 * Written on mardi, 20 mai 2025.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

rule_rec_ptr rule_pushnew( rule_rec_ptr top,
			   const char *s, const int ngetters, hypo_rec_ptr h ){
  bwrd_rec_ptr bwrd;
  // Set up rule sign-instance
  rule_rec_ptr rule = (rule_rec_ptr)sign_pushnew( top, s,
						  ngetters, sizeof(cond_rec_ptr),
						  0, sizeof(void *) );
  rule->len_type = (unsigned short)strlen( s ) | RULE_MASK;
  rule->getters  = (empty_ptr *)0;
  // Set up hypothesis in `setters` field, keeping `nsetters` to 0
  rule->setters = (empty_ptr *)h;
  // Point back from hypo to rules
  sign_pushgetter( h, (empty_ptr)malloc( sizeof(struct bwrd_rec) ) );
  bwrd = (bwrd_rec_ptr) (h->getters)[_LAST_RULE(h)];
  bwrd->rule = rule;
  return rule;
}

void rule_pushnewcond( rule_rec_ptr rule, unsigned short op, sign_rec_ptr sign ){
  cond_rec_ptr cond;
  fwrd_rec_ptr fwrd;
  sign_pushgetter( rule, (empty_ptr)malloc( sizeof(struct cond_rec) ) );
  cond = (cond_rec_ptr)  (rule->getters)[_LAST_COND(rule)];
  cond->in = cond->out = op;
  cond->val = (unsigned short)0xff;
  cond->sign = sign;
  // Point back from sign to cond
  printf( "> Nsetters: %d. Pushing fwrd_rec: %s -> %s\t", sign->nsetters,
	  sign->str,
	  ((hypo_rec_ptr)(rule->setters))->str );
  sign_pushsetter( sign, (empty_ptr)malloc( sizeof(struct fwrd_rec) ) );
  fwrd = (fwrd_rec_ptr) (sign->setters)[_LAST_FWRD(sign)];
  fwrd->rule = rule;
  fwrd->idx_cond = _LAST_COND(rule);
  printf( "Nsetters: %d\n", sign->nsetters );
}

void rule_del( rule_rec_ptr rule ){
  if( rule->ngetters )
    for( unsigned short i=0; i<rule->ngetters; i++ ){ free( (void *) (rule->getters)[i] ); }
  if( rule->getters ) free( rule->getters );
  free( rule );
}

void rule_print( rule_rec_ptr rule ){
  char *esc = S_val_color( rule->val );
  short len = rule->len_type & RULE_UNMASK;
  printf( "%sRULE:\t%s (%d, %d, %d)\t%s (Val %d)\n", esc, rule->str,
	  len, rule->len_type, rule->len_type & TYPE_MASK,
	  _VALSTR(rule->val),
	  rule->val );
  printf( "\tGetters: %d, Setters: %d\n", rule->ngetters, rule->nsetters );
  if( rule->setters ){
    hypo_print( (hypo_rec_ptr)rule->setters );
  }
  esc = S_val_color( _UNKNOWN );
  printf( "%s\tCOND: %d (%d)\n", esc, rule->ngetters, sizeof((rule->getters)) );
  if( rule->getters ){
    for( unsigned short i = 0; i<rule->ngetters; i++ ){
      printf( "\t\tCOND %d: %d == %s\tVal: %d\n", i,
	      (_AS_COND_ARRAY(rule->getters)[i])->out,
	      (_AS_COND_ARRAY(rule->getters)[i])->sign->str,
	      (_AS_COND_ARRAY(rule->getters)[i])->val);
      sign_rec_ptr sign = (_AS_COND_ARRAY(rule->getters)[i])->sign;
      for( unsigned short j = 0; j < sign->nsetters; j++ ){
	printf( "\t\tin %s at %d\n",
		((fwrd_rec_ptr)(sign->setters)[j])->rule->str,
		((fwrd_rec_ptr)(sign->setters)[j])->idx_cond );
      }
    }
  }
  printf( "%s\n", esc );
}
