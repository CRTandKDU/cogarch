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

void del_signs(){
  sign_rec_ptr prev, s = S_Signs;
  while( s ){
    //    sign_print( s );
    prev = s;
    s = s->next;
    sign_del( prev );
  }
}

void del_rules(){
  rule_rec_ptr prev, s = S_Rules;
  while( s ){
    //    rule_print( s );
    prev = s;
    s = s->next;
    rule_del( prev );
  }
}

void del_hypos(){
  hypo_rec_ptr prev, s = S_Hypos;
  while( s ){
    //    hypo_print( s );
    prev = s;
    s = s->next;
    hypo_del( prev );
  }
}

int main( int argc, char *argv[] ){
  sign_rec_ptr s_c1, s_c2;
  S_Signs = sign_pushnew( S_Signs, "CRT_KDU",
			  0, sizeof(void *),
			  1, sizeof(fwrd_rec_ptr) );
  s_c1 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "TASK",
			  0, sizeof(void *),
			  1, sizeof(fwrd_rec_ptr) );
  s_c2 = S_Signs;
  S_Signs = sign_pushnew( S_Signs, "TANK_P1",
			  0, sizeof(void *),
			  1, sizeof(fwrd_rec_ptr) );
  S_Signs = sign_pushnew( S_Signs, "TANK_P2",
			  0, sizeof(void *),
			  1, sizeof(fwrd_rec_ptr) );
  S_Signs = sign_pushnew( S_Signs, "A_LONG_SIGN",
			  0, sizeof(void *),
			  1, sizeof(fwrd_rec_ptr) );
  //
  S_Hypos = hypo_pushnew( S_Hypos, "ALARM_P1", 1 );
  //
  cond_rec_ptr c1, c2;

  S_Rules = rule_pushnew( S_Rules, "RULE_1", 2, S_Hypos );
  (S_Rules->getters)[0] = (empty_ptr)malloc( sizeof(struct cond_rec) );
  c1 = (cond_rec_ptr)  (S_Rules->getters)[0];
  c1->in = c1->out = (unsigned short)1; c1->val = (unsigned short)0xff;
  c1->sign = s_c1;
  (S_Rules->getters)[1] = (empty_ptr)malloc( sizeof(struct cond_rec) );
  c2 = (cond_rec_ptr)  (S_Rules->getters)[1];
  c2->in = c2->out = (unsigned short)1; c2->val = (unsigned short)0xff;
  c2->sign = s_c2;

  rule_print( S_Rules );
  /* sign_iter( S_Signs, &sign_print ); */
  
  free( (void *)c2 );
  free( (void *)c1 );
  //
  del_rules();
  del_signs();
  del_hypos();
  return 0;
}
