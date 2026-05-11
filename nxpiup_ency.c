/**
 * nxpiup_ency.c -- Encyclopedia management
 *
 * Written on 2026-05-09.
 */
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iup.h>
#include <iupcontrols.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "nxpiup.h"

#define NXPIUP_RED	"255 0 0"
#define NXPIUP_GREEN	"0 255 0"
#define NXPIUP_BLUE	"0 0 255"

#define NXPIUP_UNKNOWN		"Unknown"
#define NXPIUP_KNOWN		"Known"
#define NXPIUP_KNOWNTRUE	"True"
#define NXPIUP_KNOWNFALSE	"False"
#define NXPIUP_KNOWNINT		"%d"
#define NXPIUP_KNOWNSTR		"%s"

#define NXPIUP_ENCY_WIDTH  200
#define NXPIUP_ENCY_HEIGHT  20

// USERDATA struct for encyclopediae
struct ency_rec{
  int size;
  sign_rec_ptr *seq;
  int selected;
};
typedef struct ency_rec *ency_rec_ptr;

int S_LineClicked = -1;

ency_rec_ptr nxpiup_ency__newrec( int size ){
  ency_rec_ptr userdata = (ency_rec_ptr) malloc( sizeof( struct ency_rec ) );
  userdata->size	= size;
  userdata->selected	= -1;
  userdata->seq		= (sign_rec_ptr *) malloc( size * sizeof( sign_rec_ptr ) );
  return userdata;
}

void nxpiup_ency__freerec( ency_rec_ptr userdata ){
  // Voids argument `userdata'
  if( userdata->seq ) free( (void *)userdata->seq );
  free( userdata );
}

void nxpiup_ency__fgcolor( sign_rec_ptr sign, char *scolor ){
  struct val_rec val = sign->val;
  if( _UNKNOWN == val.status ){
    strcpy( scolor, IupGetGlobal( "DLGFGCOLOR" ) );
    return;
  }
  //
  switch( val.type ){
  case _VAL_T_BOOL:
    strcpy( scolor, (_FALSE == val.val_bool) ? NXPIUP_RED : NXPIUP_GREEN );
    break;
    
  case _VAL_T_INT:
    strcpy( scolor, NXPIUP_BLUE );
    break;
    
  case _VAL_T_FLOAT:
     break;
     
  case _VAL_T_STR:
    if( val.valptr )
      strcpy( scolor, NXPIUP_BLUE );
    break;
  }
  return;
}

long int nxpiup_ency__textcolor( sign_rec_ptr sign ){
  long int text_color = CD_BLACK;
  struct val_rec val = sign->val;
  if( _KNOWN == val.status ){
    switch( val.type ){
    case _VAL_T_BOOL:
      text_color = (_FALSE == val.val_bool) ? CD_RED : CD_GREEN;
      break;
      
    case _VAL_T_INT:
      text_color = CD_BLUE;
      break;
      
    case _VAL_T_FLOAT:
      break;
      
    case _VAL_T_STR:
      if( val.valptr )
	text_color = CD_BLUE;
      break;
    }
  }
  return text_color;
}

void nxpiup_ency__valuestr( sign_rec_ptr sign, char *svalue ){
  struct val_rec val = sign->val;
  if( _UNKNOWN == val.status ){
    strcpy( svalue, NXPIUP_UNKNOWN );
    return;
  }
  //
  switch( val.type ){
  case _VAL_T_BOOL:
    strcpy( svalue, (_FALSE == val.val_bool) ? NXPIUP_KNOWNFALSE : NXPIUP_KNOWNTRUE );
    break;
    
  case _VAL_T_INT:
    sprintf( svalue, NXPIUP_KNOWNINT, val.val_int );	    
    break;
    
  case _VAL_T_FLOAT:
    break;
     
  case _VAL_T_STR:
    if( val.valptr )
      sprintf( svalue, NXPIUP_KNOWNSTR, val.valptr );	    
    break;
  }
  return;
}

int ency_destroy_cb( Ihandle *ih ){
  ency_rec_ptr userdata = (ency_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  if( userdata ) nxpiup_ency__freerec( userdata );
  return IUP_DEFAULT;
}

int nxpiup_ency__compare( const void *arg1, const void *arg2 ){
  return strcmp( (* ((sign_rec_ptr *) arg1))->str, (* ((sign_rec_ptr *) arg2))->str );
}

void nxpiup_ency_update( Ihandle * ih ){
  char buf[NXPIUP_TEMP_BUFSIZE] = {0};
  char val[NXPIUP_TEMP_BUFSIZE] = {0};
  sign_rec_ptr sign;
  ency_rec_ptr userdata = (ency_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  for( short i=0; i<userdata->size; i++ ){
    sign = (sign_rec_ptr) userdata->seq[i];
    nxpiup_ency__valuestr( sign, val );
    sprintf( buf, "%-32.32s  %16s", sign->str, val );
    sprintf( val, "%d", i+1 );
    IupSetAttribute( ih, val, buf );
    *buf = 0x00;
    sprintf( buf, "ITEMFGCOLOR%d", i+1 );
    nxpiup_ency__fgcolor( sign, val );
    IupSetAttribute( ih, buf, val );
  }
}

void nxpiup_dlgency( const char *ency_title, const char *ency_handle, sign_rec_ptr top ){
  // IupListbox implementation
  Ihandle *dlg = IupGetHandle( ency_handle );
  if( !dlg ){
    if( !top ) return;
    //
    sign_rec_ptr sign;
    short item = 0;
    sign = top;
    while( sign ){
      item += 1;
      sign = sign->next;
    }
    ency_rec_ptr userdata = nxpiup_ency__newrec( item );
    item = 0;
    sign = top;
    while( sign ){
      userdata->seq[ item++ ] = sign;
      sign = sign->next;
    }
    /* printf( "QSORT pre\n" ); */
    /* for( short i=0; i<userdata->size; i++ ){ printf( "\t%s\n", userdata->seq[i]->str ); } */
    qsort( (void *) userdata->seq, (size_t) userdata->size, sizeof(sign_rec_ptr), nxpiup_ency__compare );
    /* printf( "QSORT post\n" ); */
    /* for( short i=0; i<userdata->size; i++ ){ printf( "\t%s\n", userdata->seq[i]->str ); } */
    //
    char buf[NXPIUP_TEMP_BUFSIZE] = {0};
    char val[NXPIUP_TEMP_BUFSIZE] = {0};

    Ihandle *encyh = IupFlatList();
    IupSetAttribute( encyh, "USERDATA", (char *)userdata );
    IupSetCallback( encyh, "DESTROY_CB", (Icallback) ency_destroy_cb );
    IupSetAttribute( encyh, "SIZE", "420*400" );
    IupSetAttribute( encyh, "FLATSCROLLBAR", "VERTICAL" );
    IupSetAttribute( encyh, "ALIGNMENT", "ALEFT:ACENTER" );
    IupSetAttribute( encyh, "EXPAND", "YES" );
    sprintf( buf, "%s_view", ency_handle );
    IupSetHandle( buf, encyh );
    //
    for( short i=0; i<userdata->size; i++ ){
      sign = (sign_rec_ptr) userdata->seq[i];
      nxpiup_ency__valuestr( sign, val );
      sprintf( buf, "%-32.32s  %16s", sign->str, val );
      IupSetAttribute( encyh, "APPENDITEM", buf );
      *buf = 0x00;
      sprintf( buf, "ITEMFGCOLOR%d", i+1 );
      nxpiup_ency__fgcolor( sign, val );
      IupSetAttribute( encyh, buf, val );
      sprintf( buf, "ITEMFONT%d", i+1 );
      IupSetAttribute( encyh, buf, "Courier, 12" );
    }
    //
    Ihandle *ency_vbox = IupVbox( IupFrame( encyh ), NULL );
    IupSetAttribute( ency_vbox, "MARGIN","10x10" );
    
    dlg = IupDialog( ency_vbox );
    sprintf( buf, "Encyclopedia %s", ency_title );
    IupSetAttribute( dlg, "TITLE", buf );
    IupSetAttribute( dlg, "EXPANDCHILDREN", "YES" );
    IupSetHandle( ency_handle, dlg );
  }
  IupShow( dlg );
}

void nxpiup_ency__logcond( rule_rec_ptr r, int i, char *val ){
  cond_rec_ptr cond = (cond_rec_ptr) r->getters[i];
  if( COMPOUND_MASK == (cond->sign->len_type & TYPE_MASK) ){
    char *c = ((compound_rec_ptr) cond->sign)->dsl_expression;
    for( i=0; i<NXPIUP_TEMP_BUFSIZE+NXPIUP_TEMP_BUFSIZE - 4; i++ ){
      val[i]=c[i];
      if( 0 == c[i] || '\n' == c[i] ){
	val[i] = 0x00;
	break;
      }
    }
  }
  else{
    sprintf( val, "%s %s", cond->out ? "Yes" : "No", cond->sign->str );
  }
}

void nxpiup_ency__logrule( Ihandle *ih, int id ){
  // ih is an IupMultiline w. userdata
  char buf[NXPIUP_TEMP_BUFSIZE+NXPIUP_TEMP_BUFSIZE] = {0};
  char val[NXPIUP_TEMP_BUFSIZE+NXPIUP_TEMP_BUFSIZE] = {0};
  ency_rec_ptr userdata = (ency_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  short i;
  if( id < 1 || id >= userdata->size ) return;
  
  // Title line
  IupSetAttribute( ih, "VALUE", "" );
  sprintf( buf, "Rule #%d: %s", id, userdata->seq[ id-1 ]->str );
  IupSetAttribute( ih, "APPEND", buf );
  // LHS
  IupSetAttribute( ih, "APPEND", "IF" );
  for( i=0; i<userdata->seq[ id-1 ]->ngetters; i++ ){
    nxpiup_ency__logcond( (rule_rec_ptr) userdata->seq[ id-1 ], i, val );
    if( i>0 ){
      sprintf( buf, "AND %s", val );
      IupSetAttribute( ih, "APPEND", buf );
    }
    else{
      IupSetAttribute( ih, "APPEND", val );
    }
  }
  
}

int ency_rules_valuechanged_cb( Ihandle *ih ){
  printf( "ENCY RULES %s\n", IupGetAttribute( ih, "VALUE" ) );
  Ihandle *view = IupGetHandle( NXPIUP_ENCY_RULES_VIEW );
  int id = atoi( IupGetAttribute( ih, "VALUE" ) );
  nxpiup_ency__logrule( view, id );
  return IUP_DEFAULT;
}

void nxpiup_dlgency_rules( const char *ency_title, const char *ency_handle, rule_rec_ptr top ){
  Ihandle *dlg = IupGetHandle( ency_handle );
  if( !dlg ){
    if( !top ) return;
    //
    rule_rec_ptr rule;
    short item = 0;
    rule = (rule_rec_ptr) top;
    while( rule ){
      item += 1;
      rule = (rule_rec_ptr) rule->next;
    }
    ency_rec_ptr userdata = nxpiup_ency__newrec( item );
    item = 0;
    rule = top;
    while( rule ){
      userdata->seq[ item++ ] = (sign_rec_ptr) rule;
      rule = (rule_rec_ptr) rule->next;
    }
    /* printf( "QSORT pre\n" ); */
    /* for( short i=0; i<userdata->size; i++ ){ printf( "\t%s\n", userdata->seq[i]->str ); } */
    qsort( (void *) userdata->seq, (size_t) userdata->size, sizeof(rule_rec_ptr), nxpiup_ency__compare );
    /* printf( "QSORT post\n" ); */
    /* for( short i=0; i<userdata->size; i++ ){ printf( "\t%s\n", userdata->seq[i]->str ); } */
    //
    char buf[NXPIUP_TEMP_BUFSIZE] = {0};

    Ihandle *rule_text = IupMultiLine( NULL ); // read-only
    IupSetAttribute( rule_text, "READONLY", "YES" );
    IupSetAttribute( rule_text, "SCROLLBAR", "YES" );
    IupSetAttribute( rule_text, "VISIBLELINES", "16" );
    IupSetAttribute( rule_text, "VISIBLECOLUMNS", "48" );
    IupSetAttribute( rule_text, "EXPAND", "YES" );
    IupSetAttribute( rule_text, "USERDATA", (char *) userdata );
    IupSetCallback( rule_text, "DESTROY_CB", (Icallback) ency_destroy_cb );
    sprintf( buf, "%s_view", ency_handle );
    IupSetHandle( buf, rule_text );
    
    Ihandle *rule_page = IupVal( "HORIZONTAL" );
    IupSetAttribute( rule_page, "MIN", "1" );
    sprintf( buf, "%d", userdata->size );
    IupSetAttribute( rule_page, "MAX", buf );
    IupSetAttribute( rule_page, "STEP", "1.0" );
    IupSetAttribute( rule_page, "PAGESSTEP", "1.0" );
    IupSetAttribute( rule_page, "VALUE", "1.0" );
    IupSetCallback( rule_page, "VALUECHANGED_CB", (Icallback) ency_rules_valuechanged_cb );
    
    Ihandle *ency_vbox = IupVbox( rule_text, rule_page, NULL );
    IupSetAttribute( ency_vbox, "MARGIN","10x10" );
    
    dlg = IupDialog( ency_vbox );
    sprintf( buf, "Encyclopedia %s", ency_title );
    IupSetAttribute( dlg, "TITLE", buf );
    IupSetAttribute( dlg, "EXPANDCHILDREN", "YES" );
    IupSetHandle( ency_handle, dlg );
    IupMap( dlg );
    nxpiup_ency__logrule( rule_text, 1 );
  }
  IupShow( dlg );
}

/* void nxpiup_dlgency_hypos(){ */
/*   // IupGridbox implementation */
/*   Ihandle *dlg = IupGetHandle( "ency_hypos" ); */
/*   if( !dlg ){ */
/*     sign_rec_ptr sign = (sign_rec_ptr) loadkb_get_allhypos(); */
/*     if( !sign ) return; */
/*     //  */
/*     Ihandle *encyh = IupGridBox( NULL ); */
/*     IupSetAttribute( encyh, "GAPCOL", "10" ); */
/*     IupSetAttribute( encyh, "MARGIN", "5x5" ); */
/*     IupSetAttribute( encyh, "ORIENTATION", "HORIZONTAL" ); */
/*     IupSetAttribute( encyh, "NUMDIV", "2" ); */
/*     IupSetAttribute( encyh, "NORMALIZESIZE", "HORIZONTAL" ); */
/*     IupSetHandle( "ency_hypos_view", encyh ); */
/*     // Fill */
/*     Ihandle *lbl_name, *lbl_value; */
/*     char scolor[NXPIUP_TEMP_BUFSIZE] = {0}; */
/*     while( sign ){ */
/*       lbl_name = IupLabel( sign->str ); */
/*       nxpiup_ency__valuestr( sign, scolor ); */
/*       lbl_value = IupLabel( scolor ); */
/*       *scolor = 0x00; */
/*       // */
/*       nxpiup_ency__fgcolor( sign, scolor ); */
/*       IupSetAttribute( lbl_name, "FGCOLOR", scolor ); */
/*       IupSetAttribute( lbl_value, "FGCOLOR", scolor ); */
/*       // */
/*       IupAppend( encyh, lbl_name ); */
/*       IupAppend( encyh, lbl_value ); */
/*       // */
/*       sign = sign->next; */
/*     } */
/*     // */
/*     dlg = IupDialog( IupHbox( IupFrame( encyh ), NULL ) ); */
/*     IupSetAttribute( dlg, "TITLE", "Encyclopedia Hypos"); */
/*     IupSetAttribute( dlg, "SIZE", "QUARTERxHALF" ); */
/*     IupSetHandle( "ency_hypos", dlg ); */
/*   } */
/*   IupShow( dlg ); */
  
/* } */
