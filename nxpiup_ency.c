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


int ency_nlines_cb(Ihandle* h) {
  int item = 0;
  sign_rec_ptr sign = (sign_rec_ptr) IupGetAttribute( h, "USERDATA" );
  while( sign ){
    item += 1;
    sign = sign->next;
  }
  return item;
}

int ency_ncols_cb(Ihandle* h) {return 2;}

int ency_height_cb(Ihandle* h, int i) {return 20;}

int ency_width_cb(Ihandle* h, int j) { return 200; }

int ency_draw_cb(Ihandle* h, int i, int j, int xmin, int xmax, int ymin, int ymax, cdCanvas* canvas) 
{
  /* int xm = (xmax + xmin) / 2; */
  /* int ym = (ymax + ymin) / 2; */
  /* char buffer[64]; */

  /* cdCanvasForeground(canvas, cdEncodeColor( */
  /*   (unsigned char)(i*20),  */
  /*   (unsigned char)(j*100),  */
  /*   (unsigned char)(i+100) */
  /* )); */

  /* cdCanvasBox(canvas, xmin, xmax, ymin, ymax); */
  /* cdCanvasTextAlignment(canvas, CD_CENTER); */
  /* cdCanvasForeground(canvas, CD_BLACK); */
  /* sprintf(buffer, "(%02d, %02d)", i, j); */
  /* cdCanvasText(canvas, xm, ym, buffer); */
  char buf[NXPIUP_TEMP_BUFSIZE] = {0};
  char val[NXPIUP_TEMP_BUFSIZE] = {0};
  int  item;
  sign_rec_ptr sign = (sign_rec_ptr) IupGetAttribute( h, "USERDATA" );

  cdCanvasFont( canvas, "Times", CD_PLAIN, 10 );
  for( item=0; item<i; item++ ){ sign = sign->next; }
  if( sign ){
    cdCanvasTextAlignment(canvas, CD_BASE_LEFT);
    cdCanvasForeground(canvas, nxpiup_ency__textcolor( sign ));
    switch(j){
    case 1:
      sprintf( buf, "%.32s", sign->str );
      break;
    case 2:
      nxpiup_ency__valuestr( sign, val );
      sprintf( buf, "%.16s", val );
      break;
    }
    cdCanvasText(canvas, xmin, ymin + 3, buf);
    cdCanvasForeground(canvas, CD_BLACK);
  }
  return IUP_DEFAULT;
}


void nxpiup_dlgency_hypos(){
  Ihandle *dlg = IupGetHandle( "ency_hypos" );
  if( !dlg ){
    sign_rec_ptr sign = (sign_rec_ptr) loadkb_get_allhypos();
    if( !sign ) return;
    //
    short item = 1;
    char buf[NXPIUP_TEMP_BUFSIZE] = {0};
    char val[NXPIUP_TEMP_BUFSIZE] = {0};

    Ihandle *encyh = IupCells();
    IupSetAttribute( encyh, "USERDATA", (char *)sign );
    IupSetAttribute( encyh, "BOXED", "FALSE" );
    /* IupSetCallback(cells, "MOUSECLICK_CB", (Icallback)mouseclick_cb); */
    IupSetCallback(encyh, "DRAW_CB", (Icallback)ency_draw_cb);
    IupSetCallback(encyh, "WIDTH_CB", (Icallback)ency_width_cb);
    IupSetCallback(encyh, "HEIGHT_CB", (Icallback)ency_height_cb);
    IupSetCallback(encyh, "NLINES_CB", (Icallback)ency_nlines_cb);
    IupSetCallback(encyh, "NCOLS_CB", (Icallback)ency_ncols_cb);
    IupSetHandle( "ency_hypos_view", encyh );
    //
    dlg = IupDialog( IupFrame( encyh ) );
    IupSetAttribute( dlg, "TITLE", "Encyclopedia Hypos");
    IupSetAttribute( dlg,"RASTERSIZE","400x400" );
    IupSetAttribute( dlg,"MARGIN","10x10" );
    /* IupSetAttribute( dlg, "EXPANDCHILDREN", "YES"); */
    IupSetHandle( "ency_hypos", dlg );
  }
  IupShow( dlg );
}


/* void nxpiup_dlgency_hypos(){ */
/*   // IupListbox implementation */
/*   Ihandle *dlg = IupGetHandle( "ency_hypos" ); */
/*   if( !dlg ){ */
/*     sign_rec_ptr sign = (sign_rec_ptr) loadkb_get_allhypos(); */
/*     if( !sign ) return; */
/*     // */
/*     short item = 1; */
/*     char buf[NXPIUP_TEMP_BUFSIZE] = {0}; */
/*     char val[NXPIUP_TEMP_BUFSIZE] = {0}; */

/*     Ihandle *encyh = IupFlatList(); */
/*     IupSetAttribute( encyh, "RASTERSIZE", "HALFxHALF" ); */
/*     IupSetAttribute( encyh, "FLATSCROLLBAR", "VERTICAL" ); */
/*     IupSetAttribute( encyh, "ALIGNMENT", "ALEFT:ACENTER" ); */
/*     IupSetHandle( "ency_hypos_view", encyh ); */
/*     while( sign ){ */

/*       nxpiup_ency__valuestr( sign, val ); */
/*       sprintf( buf, "%-32s  %16s", sgn->str, val ); */
/*       IupSetAttribute( encyh, "APPENDITEM", buf ); */
/*       *buf = 0x00; */
/*       sprintf( buf, "ITEMFGCOLOR%d", item ); */
/*       nxpiup_ency__fgcolor( sign, val ); */
/*       IupSetAttribute( encyh, buf, val ); */
/*       item += 1; */
/*       sign = sign->next; */
/*     } */
/*     // */
/*     dlg = IupDialog( encyh ); */
/*     IupSetAttribute( dlg, "TITLE", "Encyclopedia Hypos"); */
/*     IupSetAttribute( dlg, "EXPANDCHILDREN", "YES"); */
/*     IupSetHandle( "ency_hypos", dlg ); */
/*     IupMap(dlg); */
/*     IupSetAttribute(encyh, "RASTERSIZE", NULL);  /\* release the minimum limitation *\/ */
/*   } */
/*   IupShow( dlg ); */
/* } */

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
