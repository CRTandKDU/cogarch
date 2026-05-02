/**
 * netw.c -- Renewed experimentations with knowled base networks
 *
 * Written on 2026-04-29.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "netw.h"

#define  _NETW_NEWCELL(cptr)       (cptr) = _NEW_CELL; \
      (cptr)->y	= 0; \
      (cptr)->head = NULL; \
      (cptr)->next = NULL; \
      (cptr)->expanded = (unsigned short)0; \
      (cptr)->client_data_t = 0;\
      (cptr)->client_data = (void *)0;


static netw_cell_rec_ptr S_cellclicked = NULL;

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

int netw__trim( const char * str, cdCanvas *canvas ){
  int n, w;
  char buf[64];
  short i;
  cdCanvasGetTextSize( canvas, str, &w, NULL );
  if( w<CELL_W ) return 0;
  // The hard way
  for( i=0; i<strlen( str ); i++ ){
    buf[i] = str[i]; buf[i+1] = 0x00;
    cdCanvasGetTextSize( canvas, buf, &w, NULL );
    if( w>CELL_W ) break;
  }
  /* printf( "Trim: %s %d %d - %d %d\n", str, strlen(str), maxw, i, w ); */
  return i;
}

void netw__col_append_cell( col_rec_ptr col, netw_cell_rec_ptr cell ){
  netw_cell_rec_ptr c = col->first;
  if( c ){
    while( c->next ) c = c->next;
    c->next = cell;
  }
  else{
    col->first = cell;
  }
  cell->next = NULL;
}

int netw__col_ymax_cell( col_rec_ptr col ){
  netw_cell_rec_ptr c = col->first;
  int ymax = 0;
  while( c ){
    if( ymax < c->y ) ymax = c->y;
    c = c->next;
  }
  return ymax;
}

col_rec_ptr netw__col_get_create( col_rec_ptr head, int x ){
  // ASSERT: Cols are in increasing x-order from head
  printf( "COL_GET_CREATE %d %d >\n", head->x, x );
  netw__trace(head);
  col_rec_ptr newc, c = head;
  if( c ){
    while( c ){
      if( x == c->x ) return c;
      c = c->next;
    }
    // Not found, create
    newc = _NEW_COL;
    newc->x	= x;
    newc->first	= NULL;  
    c = head;
    while( c->next && c->next->x < x ) c = c->next;
    printf( "Inserting new c after %d before %d\n", c->x, c->next ? c->next->x : -1 ); 
    newc->next = c->next;
    c->next = newc;
  }
  else{
    newc = _NEW_COL;
    newc->x	= x;
    newc->first	= NULL;  
    newc->next	= NULL;  
  }
  netw__trace(head);
  printf( "> COL_GET_CREATE %d %d\n", head->x, newc->x );
  return newc;
}

void netw__toggle_expand( cdCanvas *canvas, netw_cell_rec_ptr cell,
			 double WORLD_W, double WORLD_H, short orientation ){
  switch( orientation ){
  case NETW_RL:
    if( _EXP_RL_P(cell) ){
      // TODO: Recursive remove
    }
    else{
      rule_rec_ptr r;
      sign_rec_ptr sign = (sign_rec_ptr) cell->client_data;
      int ncol1 = sign->ngetters;
      col_rec_ptr  head = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" );
      col_rec_ptr  col1 = netw__col_get_create( head, cell->head->x + 1 );
      col_rec_ptr  col2 = netw__col_get_create( head, cell->head->x + 2 );
      netw_cell_rec_ptr c;
      short ir, i;
      int y2, y1;
      y1 = netw__col_ymax_cell( col1 );
      y2 = netw__col_ymax_cell( col2 );
      netw__trace(head);
      printf( "ToggleExpand %s: nrules=%d, ymax1=%d, ymax2=%d\n", sign->str, y1, y2 );
      for( ir=0; ir<ncol1; ir++ ){
	r = (rule_rec_ptr) ((bwrd_rec_ptr) sign->getters[ ncol1 -1 - ir ])->rule;
	_NETW_NEWCELL( c );
	c->y = y1 + 1 + (r->ngetters + r->nrhs)/2 ;
	y1  += 1 + (r->ngetters + r->nrhs);
	c->head = col1;
	c->client_data_t = _NETW_SIGN_T;
	c->client_data   = (void *) r;
	netw__col_append_cell( col1, c );
	//
	y2 += 1;
	for( i=0; i < r->nrhs; i++ ){
	  _NETW_NEWCELL( c );
	  c->y = y2++;
	  c->head = col2;
	  c->client_data_t = _NETW_STR_T;
	  c->client_data   = (void *) r->rhs[ r->nrhs - 1 - i ];
	  netw__col_append_cell( col2, c );
	}
	for( i=0; i < r->ngetters; i++ ){
	  _NETW_NEWCELL( c );
	  c->y = y2++;
	  c->head = col2;
	  c->client_data_t = _NETW_SIGN_T;
	  c->client_data   = (void *) ((cond_rec_ptr) r->getters[ r->ngetters - 1 - i ])->sign;
	  netw__col_append_cell( col2, c );
	}
      }
      _EXP_RL_SET(cell);
    }
    break;
  case NETW_LR:
    break;
  }
}

int netw__findcell( col_rec_ptr head, double xw, double yw, double WORLD_W, double WORLD_H,
		    netw_cell_rec_ptr *result_cell ){
  col_rec_ptr  col = head;
  netw_cell_rec_ptr cell;
  while( col ){
    if( ( xw < WORLD_W - CELL_W*col->x ) && ( WORLD_W - CELL_W*col->x - CELL_W < xw ) ){
      cell = col->first;
      while( cell ){
	if( ( cell->y*CELL_H < yw ) && ( yw < cell->y*CELL_H + CELL_H ) ){
	  *result_cell = cell;
	  return 1;
	}
	cell = cell->next;
      }
    }
    col = col->next;
  }
  return 0;
}

int netw_click( cdCanvas *canvas, int but, int press, int x, int y,
		double WORLD_W, double WORLD_H, unsigned short orientation ){
  double xw, yw;
  netw_cell_rec_ptr cell;
  int needredraw = 0;
  wdCanvasCanvas2World( canvas, x, y, &xw, &yw);
  printf( "Converts %f %f\n", xw, yw );
  if( netw__findcell( (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" ), xw, yw, WORLD_W, WORLD_H, &cell ) ){
    printf( "Found at col=%d, cell=%d, %s\n", cell->head->x, cell->y, ((sign_rec_ptr)cell->client_data)->str );
    if( press && NULL == S_cellclicked ){
      S_cellclicked = cell;
    }
    else if ( !press && S_cellclicked ){
      if( S_cellclicked == cell ){
	printf( "Clicked in col=%d, cell=%d, %s\n", cell->head->x, cell->y, ((sign_rec_ptr)cell->client_data)->str );
	netw__toggle_expand( canvas, cell, WORLD_W,  WORLD_H, NETW_RL );
	needredraw = 1;
      }
      else{
	printf( "No click\n" );
      }
      S_cellclicked = NULL;
    }
  }
  else{
    if( !press ) S_cellclicked = NULL;
  }
  return needredraw;	
}


void netw_initfill_all( cdCanvas *canvas ){
  sign_rec_ptr s, top = (sign_rec_ptr) loadkb_get_allhypos();
  col_rec_ptr col = _NEW_COL;
  col->x	= 1;
  col->next	= NULL;
  netw_cell_rec_ptr cptr;
  short i;
  for( s=top, i=0, cptr = (netw_cell_rec_ptr)col; s; s=s->next  ){
    if( 0 == s->nsetters ){
      // Final hypothesis
      /* cptr->next		= _NEW_CELL; // Fields in col_rec and netw_cell_rec match! */
      /* cptr->next->y		= i+1; */
      /* cptr->next->head		= col; */
      /* cptr->next->next		= NULL; */
      /* cptr->next->expanded	= (unsigned short)0; */
      _NETW_NEWCELL( cptr->next );
      cptr->next->y		= i+1;
      cptr->next->head		= col;
      cptr->next->client_data_t = _NETW_SIGN_T;
      cptr->next->client_data	= (void *)s;
      i += 1;
      cptr=cptr->next;
    }
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
  int xv, yv, p;
  char buf[64]={0};

  cdCanvasFont( canvas, "Times", CD_PLAIN, 10 );
  cdCanvasTextAlignment( canvas, CD_SOUTH_EAST );
  col = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" ); 
  while( col ){
    /* printf( "REDRAW col %d\n", col->x ); */
    cell = col->first;
    while( cell ){
      switch( cell->client_data_t ){
      case _NETW_SIGN_T:
	s = (sign_rec_ptr) cell->client_data;
	sprintf( buf, "(%d) %s (%d)", s->ngetters, s->str, s->nsetters );
	break;
      case _NETW_STR_T:
	snprintf( buf, 64, "%s", (char *) cell->client_data );
	break;
      }
      /* printf( "REDRAW %s\n", buf ); */
      wdCanvasWorld2Canvas( canvas, (double) (WORLD_W - CELL_W*col->x), (double) (CELL_H*cell->y), &xv, &yv);
      /* cdCanvasRect( canvas, xv - CELL_W, xv, yv, yv + CELL_H ); */
      if( p = netw__trim( buf, canvas ) ) buf[p] = 0x00;
      cdCanvasText( canvas, xv, yv, buf );
      cell = cell->next;
    }
    col = col->next;
  }
}
