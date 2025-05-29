/**
 * compound.c -- DSL expressions
 *
 * Written on jeudi, 29 mai 2025.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

extern  sign_getter_t getter_compound;

compound_rec_ptr compound_pushnew( sign_rec_ptr top,
				   const char *s, const int ngetters ){
  compound_rec_ptr compound;
  unsigned short len;

  compound			= (compound_rec_ptr) malloc( sizeof( struct compound_rec ) );
  compound->next		= top;
  compound->val		= 0xff;
  len			= (unsigned short)strlen( s );
  compound->len_type	= (len < 9) ? len : 8;
  char *to		= compound->str;
  char *from		= (char *)s;
  for( unsigned short i = 0; i < compound->len_type; *to++ = *from++, i++ ); *to = 0;
  compound->len_type 	|= COMPOUND_MASK;
  compound->ngetters	= ngetters;
  compound->nsetters	= 0;
  compound->getters = (empty_ptr *) &getter_compound;
  compound->setters = (empty_ptr *)NULL;
  
  return compound;
}

