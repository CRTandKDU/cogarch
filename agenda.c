/**
 * agenda.c -- NXP Agenda Process
 *
 * Written on mardi, 20 mai 2025.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agenda.h"

static sign_rec_ptr S_Signs = (sign_rec_ptr)0;
static rule_rec_ptr S_Rules = (rule_rec_ptr)0;
static hypo_rec_ptr S_Hypos = (hypo_rec_ptr)0;

#define _DEL_SIGNS ( sign_iter(S_Signs,&sign_del) )
#define _DEL_HYPOS ( sign_iter(S_Hypos,&hypo_del) )
#define _DEL_RULES ( sign_iter(S_Rules,&rule_del) )


int main( int argc, char *argv[] ){
  sign_rec_ptr s_c1, s_c2;
  S_Signs = sign_pushnew( S_Signs, "CRT_KDU",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  s_c1 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "TASK",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  s_c2 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "TANK_P1",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  S_Signs = sign_pushnew( S_Signs, "TANK_P2",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  S_Signs = sign_pushnew( S_Signs, "A_LONG_SIGN",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  //
  S_Hypos = hypo_pushnew( S_Hypos, "ALARM_P1", 0 );
  //
  S_Rules = rule_pushnew( S_Rules, "RULE_1", 0, S_Hypos );
  // Point to two conditions LHS
  rule_pushnewcond( S_Rules, (unsigned short)1, s_c1 );
  rule_pushnewcond( S_Rules, (unsigned short)1, s_c2 );

  sign_set_default( s_c2, sign_get_default( s_c2 ) );

  sign_iter( S_Signs, &sign_print );
  rule_print( S_Rules );
  
  
  //
  _DEL_RULES;
  _DEL_SIGNS;
  _DEL_HYPOS;
  return 0;
}
