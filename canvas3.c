#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "netw.h"

/* World:
   The canvas will be a window into that space.
   If canvas is smaller than the virtual space, scrollbars are active.
   The drawing is a red X connecting the corners of the world,
   plus a box inside the borders.

   Remember that:
   XMIN<=POSX<=XMAX-DX
*/
#define WORLD_W 600
#define WORLD_H 400
static int scale = 1;


//----------------------------------------------------------------------
// IUP GUI logic
//----------------------------------------------------------------------

static void update_scrollbar(Ihandle* ih, int canvas_w, int canvas_h)
{
  /* update page size, it is always the client size of the canvas,
     but must convert it to world coordinates.
     If you change canvas size or scale must call this function. */
  double ww, wh;
  if (scale > 0)
  {
    ww = (double)canvas_w/scale;
    wh = (double)canvas_h/scale;
  }
  else
  {
    ww = canvas_w*abs(scale);
    wh = canvas_h*abs(scale);
  }
  IupSetfAttribute(ih, "DX", "%g", ww);
  IupSetfAttribute(ih, "DY", "%g", wh);
}

static void update_viewport(Ihandle* ih, cdCanvas *canvas, float posx, float posy)
{
  int view_x, view_y, view_w, view_h;

  /* The CD viewport is the same area represented by the virtual space of the scrollbar,
     but not using the same coordinates. */

  /* posy is top-bottom, CD is bottom-top.
     invert posy reference (YMAX-DY - POSY) */
  posy = IupGetFloat(ih, "YMAX")-IupGetFloat(ih, "DY") - posy;
  if (posy < 0) posy = 0;

  if (scale > 0)
  {
    view_w = WORLD_W*scale;
    view_h = WORLD_H*scale;
    view_x = (int)(posx*scale);
    view_y = (int)(posy*scale);
  }
  else
  {
    view_w = WORLD_W/abs(scale);
    view_h = WORLD_H/abs(scale);
    view_x = (int)(posx/abs(scale));
    view_y = (int)(posy/abs(scale));
  }

  wdCanvasViewport(canvas, -view_x, view_w-1 - view_x, -view_y, view_h-1 - view_y);
}

/************************************************************************************/

static int action(Ihandle *ih)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

printf("ACTION\n");
  cdCanvasActivate(canvas);
  cdCanvasClear(canvas);

  netw_redrawkb( canvas, WORLD_W, WORLD_H );

  /* cdCanvasForeground(canvas, CD_RED); */
  /* wdCanvasLine(canvas, 0, 0, WORLD_W, WORLD_H); */
  /* wdCanvasLine(canvas, 0, WORLD_H, WORLD_W, 0); */
  /* wdCanvasArc(canvas, WORLD_W/2, WORLD_H/2+WORLD_H/10, WORLD_W/10, WORLD_H/10, 0, 360); */

  /* wdCanvasLine(canvas, 0, 0, WORLD_W, 0); */
  /* wdCanvasLine(canvas, 0, WORLD_H, WORLD_W, WORLD_H); */
  /* wdCanvasLine(canvas, 0, 0, 0, WORLD_H); */
  /* wdCanvasLine(canvas, WORLD_W, 0, WORLD_W, WORLD_H); */

  return IUP_DEFAULT;
}

static int resize_cb(Ihandle *ih, int canvas_w, int canvas_h)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

printf("RESIZE_CB(%d, %d) RASTERSIZE=%s DRAWSIZE=%s \n", canvas_w, canvas_h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "DRAWSIZE"));
  /* When *AUTOHIDE=Yes, this can hide a scrollbar and so change the canvas drawsize */
  update_scrollbar(ih, canvas_w, canvas_h);  
printf("                                DRAWSIZE=%s \n", IupGetAttribute(ih, "DRAWSIZE"));
  /* update the canvas size */
  IupGetIntInt(ih, "DRAWSIZE", &canvas_w, &canvas_h);
  update_scrollbar(ih, canvas_w, canvas_h);  

  /* update the application */
  cdCanvasActivate(canvas);
  update_viewport(ih, canvas, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));

  return IUP_DEFAULT;
}

static int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
printf("SCROLL_CB(%g, %g)\n", posx, posy);
  cdCanvasActivate(canvas);
  update_viewport(ih, canvas, posx, posy);
  IupRedraw(ih, 0);
  (void)op;
  return IUP_DEFAULT;
}

static int wheel_cb(Ihandle *ih,float delta,int x,int y,char* status)
{
  int canvas_w, canvas_h;
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  (void)x;
  (void)y;
  (void)status;

  if (scale+delta==0) /* skip 0 */
  {
    if (scale > 0) 
      scale = -1;
    else 
      scale = 1;
  }
  else
    scale += (int)delta;

  cdCanvasActivate(canvas);
  cdCanvasGetSize(canvas, &canvas_w, &canvas_h, NULL, NULL);
  update_scrollbar(ih, canvas_w, canvas_h);
  update_viewport(ih, canvas, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  IupRedraw(ih, 0);
  return IUP_DEFAULT;
}

static int map_cb(Ihandle *ih)
{
  /* canvas will be automatically saved in "_CD_CANVAS" attribute */
  cdCanvas *canvas = cdCreateCanvas(CD_IUP, ih);

  /* World size is fixed */
  wdCanvasWindow(canvas, 0, WORLD_W, 0, WORLD_H);

  /* handle scrollbar in world coordinates, so we only have to update DX/DY */
  IupSetAttribute(ih, "XMIN", "0");
  IupSetAttribute(ih, "YMIN", "0");
  IupSetfAttribute(ih, "XMAX", "%d", WORLD_W);
  IupSetfAttribute(ih, "YMAX", "%d", WORLD_H);

  return IUP_DEFAULT;
}

static int unmap_cb(Ihandle *ih)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  cdKillCanvas(canvas);
  return IUP_DEFAULT;
}

void CanvasScrollbarTest(void)
{
  Ihandle *dlg, *cnv;

  cnv = IupCanvas(NULL);
  IupSetAttribute(cnv, "RASTERSIZE", "300x200"); /* initial size */
  IupSetAttribute(cnv, "SCROLLBAR", "YES");
//  IupSetAttribute(cnv, "EXPAND", "NO");

  IupSetCallback(cnv, "RESIZE_CB",  (Icallback)resize_cb);
  IupSetCallback(cnv, "ACTION",  (Icallback)action);
  IupSetCallback(cnv, "MAP_CB",  (Icallback)map_cb);
  IupSetCallback(cnv, "UNMAP_CB",  (Icallback)unmap_cb);
  IupSetCallback(cnv, "WHEEL_CB",  (Icallback)wheel_cb);
  IupSetCallback(cnv, "SCROLL_CB",  (Icallback)scroll_cb);
                   
  dlg = IupDialog(IupVbox(cnv, NULL));
  IupSetAttribute(dlg, "TITLE", "Scrollbar Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");

  IupMap(dlg);
  IupSetAttribute(cnv, "RASTERSIZE", NULL);  /* release the minimum limitation */
 
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
//----------------------------------------------------------------------
// Minimal setup and ancillaries for engine
//----------------------------------------------------------------------
engine_state_rec_ptr S_State;
engine_state_rec_ptr repl_getState(){ return S_State; }

void print_local_val_repr( struct val_rec *val ){
  if( _UNKNOWN == val->status ){
    printf( "UNKNOWN" );
    return;
  }
  
  switch( val->type ){
  case _VAL_T_BOOL:
    printf( _FALSE == val->val_bool ? "FALSE"  : "TRUE" );
    return;
    break;
    
  case _VAL_T_INT:
    printf( "%d", val->val_int );
    return;
    break;
    
  case _VAL_T_FLOAT:
     break;
     
  case _VAL_T_STR:
    if( val->valptr )
      printf( val->valptr );
    else
      printf( "VAL_T_STR error" );
    return;
    break;
  }
  printf( "Error in printing value" );
  return;
}

const char *S_Color[] = { "\x1b[38;5;46m", "\x1b[38;5;160m", "\x1b[38;5;15m" };

char *S_val_color( unsigned short val ){
  char *esc;
  switch( val ){
  case _TRUE:
    esc = (char *) S_Color[0];
    break;
  case _FALSE:
    esc = (char *) S_Color[1];
    break;
  default:
    esc = (char *) S_Color[2];
  }
  return esc;
}

void getter_sign( sign_rec_ptr sign, int *suspend ){
  printf( "Question %s\n", sign->str );
  *suspend = _TRUE;
}

static  struct val_rec v_true  = { _KNOWN, _VAL_T_BOOL, (char *)0, _TRUE, 0, 0.0, 0 };
static  struct val_rec v_false = { _KNOWN, _VAL_T_BOOL, (char *)0, _FALSE, 0, 0.0, 0 };

void engine_dsl_getter_compound( compound_rec_ptr compound, int *suspend ){
#ifdef ENGINE_DSL_HOWERJFORTH
  if( _KNOWN == compound->val.status ) return;
  
  int  err;
  printf( "Getter compound %s (%d)\n", compound->str,
	   // (char *) (compound->dsl_expression)
	   *suspend
	   );
  /* repl_log( buf ); */
  // printf( buf );
  // WHY?
  // fixCR( compound->dsl_expression );
  int r = engine_dsl_eval_async( (const char *) compound->dsl_expression, &err, suspend );

  printf( "FORTH Res %d Err %d Susp %d\n", r, err, *suspend );
  /* repl_log( buf ); */
  // printf( buf );
  printf( "Post-eval compound %s (%d)\n", compound->str,
	   // (char *) (compound->dsl_expression)
	   *suspend
	   );
  /* repl_log( buf ); */
  switch( err ){
  case 0:
    // Ignore DSL evaluation if a question is pending! Re-evaluation will happen later.
    if( _FALSE == *suspend ){
      // sprintf( buf, "Getter compound %s (%d)\n", compound->str,
      // 	       // (char *) (compound->dsl_expression)
      // 	       *suspend
      // 	       );
      // printf( buf );
      sign_set_default( (sign_rec_ptr)compound, r ? &v_true : &v_false );
      // sprintf( buf, "Compound Status %d Type %d\n", compound->val.status, compound->val.type );
      // printf( "%s", buf );
    }
    break;
  }
  
#endif  
}

void  repl_log( const char *s ){
  printf( "Log: %s\n", s );
}

void cb_on_gate( sign_rec_ptr sign, short val ){
  printf( "Gating %s (%d) - %d", sign->str, sign->val.val_bool, val );
  //
  engine_default_on_gate( sign, val );
}

void cb_on_agenda_push( sign_rec_ptr sign, struct val_rec *val ){
  printf( "Push %s", sign->str );
  //
  engine_default_on_agenda_push( sign, val );
}

void cb_on_agenda_pop( sign_rec_ptr sign, struct val_rec *val ){
  printf( "Pop %s", sign->str );
  //
  engine_default_on_agenda_pop( sign, val );
}

void cb_on_set( sign_rec_ptr sign, struct val_rec *val ){
  printf( "Set %s:\t", sign->str );
  print_local_val_repr( &sign->val ); printf("\t");
  print_local_val_repr( val ); printf("\n");
}

void cb_on_endsession( sign_rec_ptr sign, struct val_rec *val ){
  printf( "End of session." );
}


int main(int argc, char* argv[])
{
  //----------------------------------------------------------------------
  // NXP prologue
  //----------------------------------------------------------------------

  S_State		= (engine_state_rec_ptr)malloc( sizeof( struct engine_state_rec ) );
  S_State->current_sign = (sign_rec_ptr)0;
  S_State->agenda	= (cell_rec_ptr)0;
  engine_register_effects( &engine_default_on_get,
			   &cb_on_set,
			   &cb_on_gate,
			   &cb_on_agenda_push,
			   &cb_on_agenda_pop,
			   &cb_on_endsession
			   );

  // Set up DSL
#ifdef ENGINE_DSL
  engine_dsl_init();
#endif

  int err = loadkb_file( "satfault.org" );
  printf( "Loaded KB: %d\n", err );

  //----------------------------------------------------------------------
  // IUP application
  //----------------------------------------------------------------------

  IupOpen(&argc, &argv);

  CanvasScrollbarTest();

  IupMainLoop();

  IupClose();

  //----------------------------------------------------------------------
  // NXP epilogue
  //----------------------------------------------------------------------

#ifdef ENGINE_DSL
  engine_dsl_free();
#endif
  loadkb_reset();
  engine_free_state( S_State );

  return EXIT_SUCCESS;
}
#endif
