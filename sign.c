/**
 * sign.c -- Hypos and leaves in the or-and trees.
 *
 * Written on mardi, 20 mai 2025.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"


extern effect S_on_get;
extern effect S_on_set;

// Builders
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
  sign->len_type	= (len <= _CHOP) ? len : _CHOP;
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
  // Default getter for a sign
  else sign->getters = (empty_ptr *) &getter_sign;
  
  sign->setters         = nsetters ? (empty_ptr *)0 : (empty_ptr *)malloc( nsetters*size_setter );
  
  /* printf( "\tsign allocation (%s): g %d, s %d\n", s, ngetters*size_getter, nsetters*size_setter ); */
  //
  return sign;
}

void sign_del( sign_rec_ptr sign ){
  if(TRACE_ON) printf( "Deleting %s\n", sign->str );
  if( COMPOUND_MASK == (sign->len_type & TYPE_MASK) )
    compound_del( (compound_rec_ptr) sign );
  if( sign->ngetters )
    for( unsigned short i=0; i<sign->ngetters; i++ ){ free( (void *) (sign->getters)[i] ); }
  if( sign->nsetters )
    for( unsigned short i=0; i<sign->nsetters; i++ ){ free( (void *) (sign->setters)[i] ); }
  if( sign->ngetters && sign->getters ) free( sign->getters );
  if( sign->nsetters && sign->setters ) free( sign->setters );
  free( sign );
}

void sign_pushgetter( sign_rec_ptr sign, empty_ptr getr ){
  if( 0 == sign->ngetters ){
    sign->getters = (empty_ptr *)malloc( sizeof(empty_ptr) );
  }
  else{
    sign->getters = (empty_ptr *)realloc( sign->getters, (1 + sign->ngetters)*sizeof(empty_ptr) );
  }
  (sign->getters)[ sign->ngetters ] = getr;
  sign->ngetters += 1;
}

void sign_pushsetter( sign_rec_ptr sign, empty_ptr setr ){
  if( 0 == sign->nsetters ){
    sign->setters = (empty_ptr *)malloc( sizeof(empty_ptr) );
  }
  else{
    sign->setters = (empty_ptr *)realloc( sign->setters, (1 + sign->nsetters)*sizeof(empty_ptr) );
  }
  (sign->setters)[ sign->nsetters ] = setr;
  sign->nsetters += 1;
}

// I/O
unsigned short sign_get_default( sign_rec_ptr sign ){
  char buf[32] = { 0 };
  if( S_on_get ) S_on_get( sign, _UNKNOWN );
  if(TRACE_ON) printf( "Q: What is the value of %s?\n(Type 0 or 1)\nA: ", sign->str );
  fgets( buf, 30, stdin );
  return (unsigned short)strtoul( buf, NULL, 0 );
}

void sign_set_default( sign_rec_ptr sign, unsigned short val ){
  sign->val = val;
  if( S_on_set ) S_on_set( sign, val );
}

// Default sync sign-getter
void getter_sign( sign_rec_ptr sign ){
  if(TRACE_ON) printf ("__FUNCTION__ = %s\n", __FUNCTION__);
  sign_set_default( sign, sign_get_default( sign ) );
}

// Managers
void sign_print( sign_rec_ptr sign ){
  char *esc = S_val_color( sign->val );
  short len = sign->len_type & SIGN_UNMASK;
  if(TRACE_ON) printf( "%sSIGN:\t%s (%d, %d, %d)\t%s (Val %d)\n", esc, sign->str,
	  len, sign->len_type, sign->len_type & TYPE_MASK,
	  _VALSTR(sign->val),
	  sign->val );
  if(TRACE_ON) printf( "\tGetters %d (%d), Setters %d (%d)\n",
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

sign_rec_ptr sign_find ( const char *str, sign_rec_ptr top ){
  sign_rec_ptr s = top, res = (sign_rec_ptr)NULL;
  while( s ){
    if( 0 == strcmp( s->str, str ) ){
      res = s;
      break;
    }
    s = s->next;
  }
  return res;
}

hypo_rec_ptr sign_tohypo( hypo_rec_ptr hypo, sign_rec_ptr top_sign, hypo_rec_ptr top_hypo ){
  sign_rec_ptr s = top_sign;
  if(TRACE_ON) printf( "Changing sign %s to hypo\n", hypo->str );
  hypo->len_type = (unsigned short)strlen(hypo->str) | HYPO_MASK;
  while( hypo != s->next ) s = s->next;
  s->next = hypo->next;
  hypo->next = top_hypo;
  return hypo;
}

