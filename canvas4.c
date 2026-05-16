/*
 * IupCanvas Redraw example
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <iup.h>
#include <iupcontrols.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "layout.h"

struct layout_rec{
  void *g;
  void *names;
  void *positions;
};

typedef struct layout_rec layout_rec, *layout_rec_ptr;

#define _MARK_SIZE 5
#define WORLD_W 220
#define WORLD_H 220

Ihandle *dlg     = NULL;
Ihandle *bt      = NULL;
Ihandle *gauge   = NULL;
Ihandle *tabs    = NULL;
Ihandle *cv      = NULL;
cdCanvas*cdcanvas= NULL;

int need_redraw, redraw_count = 0;

void redrawv_cb( char *, double, double );
void redrawe_cb( char *, double, double, char *, double, double );
int redraw( Ihandle * );

// From canvas3
int scale = 1;

void update_scrollbar(Ihandle* ih, int canvas_w, int canvas_h)
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

void update_viewport(Ihandle* ih, cdCanvas *canvas, float posx, float posy)
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

int resize_cb(Ihandle *ih, int canvas_w, int canvas_h)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

  printf("RESIZE_CB(%d, %d) RASTERSIZE=%s DRAWSIZE=%s \n", canvas_w, canvas_h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "DRAWSIZE"));
  /* When *AUTOHIDE=Yes, this can hide a scrollbar and so change the canvas drawsize */
  update_scrollbar(ih, canvas_w, canvas_h);  
  /* printf("                                DRAWSIZE=%s \n", IupGetAttribute(ih, "DRAWSIZE")); */
  /* update the canvas size */
  IupGetIntInt(ih, "DRAWSIZE", &canvas_w, &canvas_h);
  update_scrollbar(ih, canvas_w, canvas_h);

  /* update the application */
  cdCanvasActivate(canvas);
  update_viewport(ih, canvas, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));

  return IUP_DEFAULT;
}

int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  /* printf("SCROLL_CB(%g, %g)\n", posx, posy); */
  cdCanvasActivate(canvas);
  update_viewport(ih, canvas, posx, posy);
  IupRedraw(ih, 0);
  (void)op;
  return IUP_DEFAULT;
}


//
void update_redraw_cb( char *node, double x, double y ){
  printf( "\tUPDATE %s\t%f\t%f\n", node, x, y );
}

void layout_update_cb (int iter ){
  Ihandle *ih =  IupGetHandle( "layout_cv" );
  cdCanvas *canvas = (cdCanvas *) IupGetAttribute( ih, "_CD_CANVAS" );
  layout_rec_ptr userdata = (layout_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  if( userdata ){
    usleep( 50000 );
    cdCanvasActivate(canvas);
    cdCanvasClear(canvas);
    layout_enumerate_edges( (userdata->g), (userdata->names), (userdata->positions), redrawe_cb );
    layout_enumerate_vertices( (userdata->g), (userdata->names), (userdata->positions), redrawv_cb );
    IupUpdate( ih );
    /* printf( "%d ", iter ); */
  }
}


int toggle_redraw(void)
{
  Ihandle *ih =  IupGetHandle( "layout_cv" );
  cdCanvas *canvas = (cdCanvas *) IupGetAttribute( ih, "_CD_CANVAS" );
  layout_rec_ptr data = (layout_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  if( !data ){
    layout_rec_ptr userdata = (layout_rec_ptr) malloc( sizeof( layout_rec ) );
    IupSetAttribute( ih, "USERDATA", (char *) userdata );

    layout_open( &(userdata->g), &(userdata->names) );

    layout_add_edge( (userdata->g), (userdata->names), (char *) "0", (char *) "1" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "0", (char *) "2" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "0", (char *) "3" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "1", (char *) "4" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "1", (char *) "5" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "2", (char *) "6" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "3", (char *) "7" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "3", (char *) "8" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "3", (char *) "9" );
    layout_add_edge( (userdata->g), (userdata->names), (char *) "2", (char *) "9" );

    layout_run( (userdata->g), &(userdata->positions), 100, (double) 200., (double) 200., layout_update_cb );
    IupUpdate( ih );
  }
  return IUP_DEFAULT;
}

void redrawv_cb( char * node, double x, double y ){
  int xx, yy;
  Ihandle *ih =  IupGetHandle( "layout_cv" );
  cdCanvas *canvas = (cdCanvas *) IupGetAttribute( ih, "_CD_CANVAS" );
  layout_rec_ptr userdata = (layout_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  /* printf( "Point %s, x=%f, y=%f\n", node, x, y ); */
  //
  wdCanvasWorld2Canvas( canvas, x + 110, y + 110, &xx, &yy );
  cdCanvasRect( canvas,
		(int) xx - _MARK_SIZE, (int) xx + _MARK_SIZE,
		(int) yy - _MARK_SIZE, (int) yy + _MARK_SIZE );
  cdCanvasText( canvas, (int) xx + _MARK_SIZE + 2, (int) yy, node );
}

void redrawe_cb( char *source, double xs, double ys,
		 char *target, double xt, double yt ){
  int xxs, yys, xxt, yyt;
  Ihandle *ih =  IupGetHandle( "layout_cv" );
  cdCanvas *canvas = (cdCanvas *) IupGetAttribute( ih, "_CD_CANVAS" );
  layout_rec_ptr userdata = (layout_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  /* printf( "Point %s, x=%f, y=%f\n", node, x, y ); */
  //
  wdCanvasWorld2Canvas( canvas, xs + 110, ys + 110, &xxs, &yys );
  wdCanvasWorld2Canvas( canvas, xt + 110, yt + 110, &xxt, &yyt );
  cdCanvasLine( canvas, xxs, yys, xxt, yyt );
}


int layout_destroy_cb( Ihandle *ih ){
  layout_rec_ptr userdata = (layout_rec_ptr) IupGetAttribute( (Ihandle *) cv, "USERDATA" );
  if( userdata ){
    layout_close( (userdata->g), (userdata->names), (userdata->positions) );
    free( (void *) userdata );
  }
  return IUP_DEFAULT;
}

int redraw( Ihandle *ih )
{
  layout_rec_ptr userdata = (layout_rec_ptr) IupGetAttribute( ih, "USERDATA" );
  if( userdata ){
    cdCanvas *canvas = (cdCanvas *) IupGetAttribute( ih, "_CD_CANVAS" );
    cdCanvasActivate( canvas );
    cdCanvasClear( canvas );
    layout_enumerate_edges( (userdata->g), (userdata->names), (userdata->positions), redrawe_cb );
    layout_enumerate_vertices( (userdata->g), (userdata->names), (userdata->positions), redrawv_cb );
  }
  return IUP_DEFAULT;
}

void print_cb( char * node, double x, double y ){
  printf( "Point %s, x=%f, y=%f\n", node, x, y );
}

int main(int argc, char **argv) 
{
  IupOpen(&argc, &argv);
//  IupControlsOpen();
  
  gauge = IupProgressBar();
//  gauge = IupGauge();
  cv    = IupCanvas(NULL);
  bt    = IupButton("Start/Stop", NULL);
  IupSetAttribute(bt,    "SIZE", "50x50");
  IupSetAttribute(gauge, "SIZE", "200x15");
  
  IupSetAttribute( cv, "SIZE", "220x220");
  IupSetAttribute( cv, "SCROLLBAR", "YES" );
  IupSetCallback( cv, "RESIZE_CB",	(Icallback) resize_cb);
  IupSetCallback( cv, "SCROLL_CB",	(Icallback) scroll_cb);
  IupSetCallback( cv, "DESTROY_CB",	(Icallback) layout_destroy_cb);
  IupSetCallback( cv, "ACTION",		(Icallback) redraw );
  IupSetHandle( "layout_cv", cv );
  
  dlg   = IupDialog(IupVbox(cv, IupHbox(gauge, bt, NULL), NULL));
  IupSetAttribute(dlg, "TITLE", "Redraw test");

  IupMap(dlg);
  
  cdCanvas *cdCanvas = cdCreateCanvas(CD_IUP, cv);
  wdCanvasWindow(cdCanvas, 0, WORLD_W, 0, WORLD_H);
  cdCanvasForeground(cdcanvas, CD_BLUE);
  cdCanvasClear(cdcanvas);
  /* World size is fixed */

  /* handle scrollbar in world coordinates, so we only have to update DX/DY */
  IupSetAttribute(cv, "XMIN", "0");
  IupSetAttribute(cv, "YMIN", "0");
  IupSetfAttribute(cv, "XMAX", "%d", WORLD_W);
  IupSetfAttribute(cv, "YMAX", "%d", WORLD_H);
  
  IupSetCallback(bt, "ACTION", (Icallback)toggle_redraw);
  //
  /* layout_enumerate_vertices( (userdata->g), (userdata->names), (userdata->positions), print_cb ); */
  //
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop();
  //
  //
  IupClose();

  return EXIT_SUCCESS;
}
