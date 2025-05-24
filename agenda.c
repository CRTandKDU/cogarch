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
engine_state_rec_ptr S_State;

#define _DEL_SIGNS ( sign_iter(S_Signs,&sign_del) )
#define _DEL_HYPOS ( sign_iter(S_Hypos,&hypo_del) )
#define _DEL_RULES ( sign_iter(S_Rules,&rule_del) )


int main( int argc, char *argv[] ){
  // New state
  S_State		= (engine_state_rec_ptr)malloc( sizeof( struct engine_state_rec ) );
  S_State->current_sign = (sign_rec_ptr)0;
  S_State->agenda	= (cell_rec_ptr)0;
  engine_register_effects( &engine_default_on_get,
			   &engine_default_on_set,
			   &engine_default_on_gate);

  // KB
  sign_rec_ptr s_c1, s_c2, s_c3, s_cp1;
  hypo_rec_ptr h_1;
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
  s_cp1 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "TANK_P2",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  s_c3 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "A_LONG_SIGN",
			  0, sizeof(void *),
			  0, sizeof(fwrd_rec_ptr) );
  //
  h_1 = S_Hypos = hypo_pushnew( S_Hypos, "ALARM_P1", 0 );
  //
  S_Rules = rule_pushnew( S_Rules, "RULE_1", 0, S_Hypos );
  // Point to two conditions LHS
  rule_pushnewcond( S_Rules, (unsigned short)1, s_c1 );
  rule_pushnewcond( S_Rules, (unsigned short)1, s_c2 );

  S_Rules = rule_pushnew( S_Rules, "RULE_2", 0, S_Hypos );
  // Point to two conditions LHS
  rule_pushnewcond( S_Rules, (unsigned short)1, s_c3 );

  S_Hypos = hypo_pushnew( S_Hypos, "ALERT", 0 );
  rule_pushnewcond( S_Rules, (unsigned short)1, S_Hypos );
  S_Rules = rule_pushnew( S_Rules, "RULE_3", 0, S_Hypos );
  // Point to two conditions LHS
  rule_pushnewcond( S_Rules, (unsigned short)1, S_Signs );
  rule_pushnewcond( S_Rules, (unsigned short)1, s_cp1 );
  

  /* engine_backward_hypo( S_Hypos ); */
  /* engine_pushnew_hypo( S_State, S_Hypos ); */
  engine_pushnew_signdata( S_State, s_c1, _TRUE );
  engine_print_state( S_State );
  engine_knowcess( S_State );

  sign_iter( S_Signs, &sign_print );
  printf( "%s----\t----\t----\n", S_val_color(_UNKNOWN) );
  /* rule_print( S_Rules ); */
  sign_iter( S_Rules, &rule_print );
  
  //
  _DEL_RULES;
  
  _DEL_SIGNS;
  _DEL_HYPOS;
  engine_free_state( S_State );
  
  return 0;
}
