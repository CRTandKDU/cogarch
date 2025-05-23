#ifndef AGENDA_H
#define  AGENDA_H

struct empty {};
typedef struct empty *empty_ptr;

struct sign_rec;
typedef struct sign_rec *sign_rec_ptr;
typedef void (*sign_op) (sign_rec_ptr);

/* struct rule_rec; */
/* typedef struct rule_rec *rule_rec_ptr; */
/* struct hypo_rec; */
/* typedef struct hypo_rec *hypo_rec_ptr; */
typedef sign_rec_ptr rule_rec_ptr;
typedef sign_rec_ptr hypo_rec_ptr;

#define SIGN_MASK   (unsigned short)0x00
#define SIGN_UNMASK (unsigned short)0x0F
#define RULE_MASK   (unsigned short)0x40
#define RULE_UNMASK (unsigned short)0x0F
#define HYPO_MASK   (unsigned short)0x80
#define HYPO_UNMASK (unsigned short)0x0F

#define TYPE_MASK	 (unsigned short)0xC0

#define _TRUE		 ((unsigned short)1)
#define _FALSE		 ((unsigned short)0)
#define _UNKNOWN	 ((unsigned short)0xFF)
#define _VALSTR(val)	 (_UNKNOWN == (val) ? "UNKNOWN" : ((_TRUE==(val))?"TRUE":"FALSE"))

static char *S_Color[] = { "\x1b[38;5;46m", "\x1b[38;5;160m", "\x1b[38;5;15m" };
static char *S_val_color( unsigned short val ){
  char *esc;
  switch( val ){
  case _TRUE:
    esc = S_Color[0];
    break;
  case _FALSE:
    esc = S_Color[1];
    break;
  default:
    esc = S_Color[2];
  }
  return esc;
}

//
struct fwrd_rec {
  // Rule rec pointer
  rule_rec_ptr rule;
  int idx_cond;
};
typedef struct fwrd_rec *fwrd_rec_ptr;

struct sign_rec {
  sign_rec_ptr   next;
  unsigned short val;
  unsigned short len_type;
  char           str[9];
  int            ngetters;
  empty_ptr      *getters;
  int            nsetters;
  empty_ptr      *setters;
};

sign_rec_ptr sign_pushnew	( sign_rec_ptr top,
				  const char *s,
				  const int ngetters, const size_t size_getter,
				  const int nsetters, const size_t size_setter
				  );
void sign_del			( sign_rec_ptr sign );
void sign_pushgetter		( sign_rec_ptr sign, empty_ptr getr );
void sign_pushsetter		( sign_rec_ptr sign, empty_ptr setr );
unsigned short sign_get_default	( sign_rec_ptr sign );
void sign_set_default           ( sign_rec_ptr sign, unsigned short val );
void sign_print			( sign_rec_ptr sign );
void sign_iter			( sign_rec_ptr s0, sign_op );
//
struct cond_rec {
  unsigned short in;
  unsigned short out;
  unsigned short val;
  sign_rec_ptr   sign;
};
typedef struct cond_rec *cond_rec_ptr;
#define _AS_COND_ARRAY(ptr) ((cond_rec_ptr *)(ptr))


#define _LAST_RULE(ptr)     ((ptr)->ngetters-1)
#define _LAST_COND(ptr)     ((ptr)->ngetters-1)
#define _LAST_FWRD(ptr)     ((ptr)->nsetters-1)

  
/* struct rule_rec { */
/*   rule_rec_ptr   next; */
/*   unsigned short val; */
/*   unsigned short len_type; */
/*   char           str[9]; */
/*   int            ngetters; */
/*   cond_rec_ptr   *getters; */
/*   hypo_rec_ptr   setters; */
/* }; */

rule_rec_ptr rule_pushnew( rule_rec_ptr top,
			   const char *s, const int ngetters, hypo_rec_ptr h );
void rule_pushnewcond( rule_rec_ptr rule, unsigned short op, sign_rec_ptr sign );
void rule_del( rule_rec_ptr rule );
void rule_print( rule_rec_ptr rule );

//
struct bwrd_rec {
  rule_rec_ptr rule;
};
typedef struct bwrd_rec *bwrd_rec_ptr;

/* struct hypo_rec { */
/*   hypo_rec_ptr   next; */
/*   unsigned short val; */
/*   unsigned short len_type; */
/*   char           str[9]; */
/*   bwrd_rec_ptr    *getters; */
/*   struct sign_rec *setters; */
/* }; */

hypo_rec_ptr hypo_pushnew( hypo_rec_ptr top,
			   const char *s, const int ngetters );
void hypo_del( hypo_rec_ptr hypo );
void hypo_print( hypo_rec_ptr hypo );

//
struct cell_rec;
typedef struct cell_rec *cell_rec_ptr;
struct cell_rec {
  cell_rec_ptr next;
  sign_rec_ptr sign_or_hypo;
  unsigned short val;
};

struct engine_state_rec {
  sign_rec_ptr current_sign;
  cell_rec_ptr agenda;
};
typedef struct engine_state_rec *engine_state_rec_ptr;

unsigned short  engine_sc_or( hypo_rec_ptr hypo );
unsigned short	engine_sc_and( rule_rec_ptr rule );
void		engine_forward_rule( rule_rec_ptr rule );
void		engine_forward_cond( rule_rec_ptr rule, cond_rec_ptr cond );
void		engine_forward_sign( sign_rec_ptr sign );
void		engine_backward_hypo( hypo_rec_ptr hypo );
void		engine_backward_rule( rule_rec_ptr rule );
void		engine_backward_cond( cond_rec_ptr cond );
void            engine_free_state( engine_state_rec_ptr state );
void            engine_print_state( engine_state_rec_ptr state );
void            engine_pushnew_hypo( engine_state_rec_ptr state, hypo_rec_ptr h );
void            engine_pushnew_signdata( engine_state_rec_ptr state, sign_rec_ptr sign, unsigned short val );
void            engine_knowcess( engine_state_rec_ptr state );

// Global
typedef void (*effect) (sign_rec_ptr);

void engine_register_effects( effect f_get, effect f_set, effect f_gate );
void engine_default_on_get( sign_rec_ptr sign );
void engine_default_on_set( sign_rec_ptr sign );
void engine_default_on_gate( sign_rec_ptr sign );

#endif

