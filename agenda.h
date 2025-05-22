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

#define TYPE_MASK   (unsigned short)0xC0

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


unsigned short	engine_sc_and( rule_rec_ptr rule );
void		engine_forward_rule( rule_rec_ptr rule );
void		engine_forward_cond( rule_rec_ptr rule, cond_rec_ptr cond );
void		engine_forward_sign( sign_rec_ptr sign );

#endif

