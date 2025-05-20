/**
 * sign.c -- Hypos and leaves in the or-and trees.
 *
 * Written on mardi, 20 mai 2025.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

sign_rec_ptr sign_pushnew( sign_rec_ptr top,
			   const char *s,
			   const int ngetters, const size_t size_getter,
			   const int nsetters, const size_t size_setter
			   ){
  sign_rec_ptr sign;
  unsigned short len;
  //
  sign			= (sign_rec_ptr) malloc( sizeof( struct sign_rec ) );;
  sign->next		= top;
  sign->val		= 0xff;
  len			= (unsigned short)strlen( s );
  sign->len_type	= (len < 9) ? len : 8;
  char *to		= sign->str;
  char *from		= (char *)s;
  for( unsigned short i = 0; i < sign->len_type; *to++ = *from++, i++ ); *to = 0;
  sign->len_type 	|= SIGN_MASK;
  sign->ngetters	= ngetters;
  sign->nsetters	= nsetters;

  if( ngetters ){
    sign->getters = (empty_ptr *)malloc( ngetters*size_getter );
    if( NULL == sign->getters ){
      fprintf( stderr, "Allocation of getters failed>\n" );
      exit( 1 );
    }
  }
  else sign->getters = (empty_ptr *)0;
  
  sign->setters         = nsetters ? (empty_ptr *)0 : (empty_ptr *)malloc( nsetters*size_setter );
  
  /* printf( "\tsign allocation (%s): g %d, s %d\n", s, ngetters*size_getter, nsetters*size_setter ); */
  //
  return sign;
}

void sign_del( sign_rec_ptr sign ){
  if( sign->getters ) free( sign->getters );
  if( sign->setters ) free( sign->setters );
  free( sign );
}

void sign_print( sign_rec_ptr sign ){
  short len = sign->len_type & SIGN_UNMASK;
  printf( "SIGN:\t%s (%d, %d, %d)\tVal %d\n", sign->str,
	  len, sign->len_type, sign->len_type & TYPE_MASK,
	  sign->val );
  printf( "\tGetters %d (%d), Setters %d (%d)\n",
	  sign->ngetters, sizeof( sign->getters ),
	  sign->nsetters, sizeof( sign->setters ) );
}

void sign_iter( sign_rec_ptr s0, sign_op func ){
  sign_rec_ptr prev, s=s0;
  while( s ){
    prev = s;
    s = s->next;
    func( prev );
  }
}
