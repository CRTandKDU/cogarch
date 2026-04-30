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

void netw__trace( col_rec_ptr col ){
  col_rec_ptr c;
  netw_cell_rec_ptr cell;
  for( c = col; c; c=c->next ){
    cell = c->first;
    while( cell ){
      printf( "Col %d: Cell %d %d %s\n", c->x, cell->head->x, cell->y, ((sign_rec_ptr) cell->client_data)->str );
      cell = cell->next;
    }
  }
}

void netw_initfill_all( cdCanvas *canvas ){
  sign_rec_ptr s, top = (sign_rec_ptr) loadkb_get_allhypos();
  col_rec_ptr col = _NEW_COL;
  col->x	= 1;
  col->next	= NULL;
  netw_cell_rec_ptr cptr;
  short i;
  for( s=top, i=0, cptr = (netw_cell_rec_ptr)col; s; s=s->next, cptr=cptr->next ){
    cptr->next = _NEW_CELL; // Fields in col_rec and netw_cell_rec match!
    cptr->next->y = i+1;
    cptr->next->head = col;
    cptr->next->next = NULL;
    cptr->next->client_data = (void *)s;
    i += 1;
  }
  cdCanvasSetAttribute( canvas, "USERDATA", (char *) col);

  /* netw__trace( (col_rec_ptr)  cdCanvasGetAttribute( canvas, "USERDATA" ) ); */
}

void netw_free( cdCanvas *canvas ){
  netw_cell_rec_ptr cell, old_cell;
  col_rec_ptr  old_col, col = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" );
  while( col ){
    cell = col->first;
    while( cell ){
      old_cell = cell;
      cell = cell->next;
      free( (void *) old_cell );
    }
    old_col = col;
    col     = col->next;
    free( (void *) old_col );
  }
}

void netw_redrawkb( cdCanvas *canvas, double WORLD_W, double WORLD_H ){
  col_rec_ptr  col;
  netw_cell_rec_ptr cell;
  sign_rec_ptr s;
  short i;
  int xv, yv;
  char buf[64]={0};

  cdCanvasFont( canvas, "Times", CD_PLAIN, 10 );
  cdCanvasTextAlignment( canvas, CD_SOUTH_EAST );
  col = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" ); 
  while( col ){
    printf( "REDRAW col %d\n", col->x );
    cell = col->first;
    while( cell ){
      s = (sign_rec_ptr) cell->client_data;
      sprintf( buf, "(%d) %s (%d)", s->ngetters, s->str, s->nsetters );
      printf( "REDRAW %s\n", buf );
      wdCanvasWorld2Canvas( canvas, (double) (WORLD_W - CELL_W*col->x), (double) (CELL_H*cell->y), &xv, &yv);
      cdCanvasText( canvas, xv, yv, buf );
      cell = cell->next;
    }
    col = col->next;
  }
}
