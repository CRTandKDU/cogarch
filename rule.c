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
  rule_rec_ptr rule = (rule_rec_ptr)sign_pushnew( top, s,
						  ngetters, sizeof(cond_rec_ptr),
						  0, sizeof(void *) );
  /* unsigned short len; */
  /* rule			= (rule_rec_ptr) malloc( sizeof( struct rule_rec ) );; */
  /* rule->next		= top; */
  /* rule->val		= 0xff; */
  /* len			= (unsigned short)strlen( s ); */
  /* rule->len_type	= (len < 9) ? len : 8; */
  /* char *to		= rule->str; */
  /* char *from		= (char *)s; */
  /* for( unsigned short i = 0; i < rule->len_type; *to++ = *from++, i++ ); *to = 0; */
  /* rule->len_type 	|= RULE_MASK; */
  /* // */
  /* rule->setters = h; */
  /* rule->ngetters = (int)ngetters; */
  /* cond_rec_ptr *getters = (cond_rec_ptr *)malloc( 1+ngetters*sizeof(cond_rec_ptr) ); */
  /* /\* for( unsigned short i = 0; i < ngetters; i++ ) getters[i] = (cond_rec_ptr)0x00; *\/ */
  /* rule->getters = getters; */
  rule->len_type = (unsigned short)strlen( s ) | RULE_MASK;
  rule->setters = (void *)h;
  return rule;
};

void rule_del( rule_rec_ptr rule ){
  if( rule->getters ) free( rule->getters );
  free( rule );
}

void rule_print( rule_rec_ptr rule ){
  short len = rule->len_type & RULE_UNMASK;
  printf( "RULE:\t%s (%d, %d, %d)\tVal %d\n", rule->str,
	  len, rule->len_type, rule->len_type & TYPE_MASK,
	  rule->val );
  printf( "\tHYPO: %d\n", rule->nsetters );
  if( rule->setters ){
    hypo_print( (hypo_rec_ptr)rule->setters );
  }
  printf( "\tCOND: %d (%d)\n", rule->ngetters, sizeof((rule->getters)) );
  if( rule->getters ){
    for( unsigned short i = 0; i<rule->ngetters; i++ ){
      printf( "\tCOND: %d == %s\n",
	      (_AS_COND_ARRAY(rule->getters)[i])->out,
	      (_AS_COND_ARRAY(rule->getters)[i])->sign->str );
    }
  }
}
