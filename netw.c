/**
 * netw.c -- Renewed experimentations with knowled base networks
 *
 * Written on 2026-04-29.
 */
#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "netw.h"

#define CELL_W 60
#define CELL_H 20

void netw_redrawkb( cdCanvas *canvas, double WORLD_W, double WORLD_H ){
  sign_rec_ptr s, top = (sign_rec_ptr) loadkb_get_allhypos();
  short i;
  int xv, yv;
  char buf[64]={0};
  
  cdCanvasFont( canvas, "Times", CD_PLAIN, 10 );
  cdCanvasTextAlignment( canvas, CD_SOUTH_EAST );
  for( s=top, i=0; s; s=s->next ){
    // Show root hypotheses
    if( 0 == s->nsetters ){
      sprintf( buf, "(%d) %s (%d)", s->ngetters, s->str, s->nsetters );
      wdCanvasWorld2Canvas( canvas, (double) (WORLD_W - CELL_W), (double) ((i+1)*CELL_H), &xv, &yv);
      cdCanvasText( canvas, xv, yv, buf );
      i += 1;
    }
    /* printf( "Hypo: %s\n", s->str ); */
  }
}
