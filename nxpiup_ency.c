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
