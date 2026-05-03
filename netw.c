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
      (cptr)->client_data = (void *)0;\
      (cptr)->nleft = 0; \
      (cptr)->left = NULL; \
      (cptr)->nright = 0; \
      (cptr)->right = NULL;

#define _NETW_INFINITY 100000
#define _NETW_TEMP_BUFSIZE 64

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
  char buf[_NETW_TEMP_BUFSIZE];
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

int netw__col_ymin_cell( col_rec_ptr col ){
  netw_cell_rec_ptr c = col->first;
  int ymin = _NETW_INFINITY;
  while( c ){
    if( ymin > c->y ) ymin = c->y;
    c = c->next;
  }
  return ymin;
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

void netw__text( cdCanvas *canvas, netw_cell_rec_ptr cell, char *buf, int *bufsize ){
  sign_rec_ptr s;
  short i;
  int p;
  switch( cell->client_data_t ){
  case _NETW_SIGN_T:
  case _NETW_RULE_T:
    s = (sign_rec_ptr) cell->client_data;
    if( COMPOUND_MASK == (s->len_type & TYPE_MASK) ){
      char *c = ((compound_rec_ptr) s)->dsl_expression;
      for( i=0; i<_NETW_TEMP_BUFSIZE; i++ ){
	buf[i]=c[i];
	if( 0 == c[i] || '\n' == c[i] ){
	  buf[i] = 0x00;
	  break;
	}
      }
    }
    else{
      sprintf( buf, "(%d) %s (%d)", s->ngetters, s->str, s->nsetters );
    }
    break;
  case _NETW_STR_T:
    char *c = (char *) cell->client_data;
    buf[0] = '='; buf[1] = '>'; buf[2] = ' ';
    for( i=0; i<_NETW_TEMP_BUFSIZE-3; i++ ){
      buf[i + 3]=c[i];
      if( 0 == c[i] || '\n' == c[i] ){
	buf[i + 3] = 0x00;
	break;
      }
    }
    break;
  }
  if( p = netw__trim( buf, canvas ) ) buf[p] = 0x00;
  cdCanvasGetTextSize( canvas, buf, bufsize, NULL );
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
      int ncol2 = 0;
      col_rec_ptr  head = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" );
      col_rec_ptr  col1 = netw__col_get_create( head, cell->head->x + 1 );
      col_rec_ptr  col2 = netw__col_get_create( head, cell->head->x + 2 );
      netw_cell_rec_ptr c, crule;
      short ir, i;
      int y1		= netw__col_ymax_cell( col1 );
      int y2		= netw__col_ymax_cell( col2 );
      int y1min		= netw__col_ymin_cell( col1 );
      int y2min		= netw__col_ymin_cell( col2 );
      printf( "ToggleExpand %s: nrules=%d, ymax1=%d, ymax2=%d\n",
	      sign->str, ncol1, y1, y2 );
      // Allocate left links in RL orientation
      cell->nleft = ncol1;
      cell->left = (netw_cell_rec_ptr *) malloc( ncol1 * sizeof(netw_cell_rec_ptr) );
      // Impossible Aesthetics!
      /* y1 = (y2) ? y2 + 1 : y2 ; */
      /* y2 = y1; */
      for( ir=0; ir<ncol1; ir++ ){
	r = (rule_rec_ptr) ((bwrd_rec_ptr) sign->getters[ ncol1 - 1 - ir ])->rule;
	ncol2 += r->nrhs + r->ngetters + 1;
      }
      printf( "ToggleExpand: nconds=%d, ymin1=%d, ymin2=%d\n",
	      ncol2, y1min, y2min );
      if( y2 ){
	if( ncol2 < y2min ){
	  y1 = y2min - ncol2 - 1;
	  y2 = y1;
	}
	else{
	  if( (cell->y - ncol2/2) > y2 ){
	    y2 = cell->y - ncol2/2 + 1;
	    y1 = y2;
	  }
	  else{
	    y1 = y2 + 1;
	    y2 = y1;
	  }
	}
      }
      else{
	y2 = (cell->y - ncol2/2) > 0 ? (cell->y - ncol2/2) : 1;
	y1 = y2;
      }
      //
      for( ir=0; ir<ncol1; ir++ ){
	r = (rule_rec_ptr) ((bwrd_rec_ptr) sign->getters[ ncol1 - 1 - ir ])->rule;
	_NETW_NEWCELL( c );
	c->y			= y1 + 1 + (r->ngetters + r->nrhs)/2 ;
	y1  += 1 + (r->ngetters + r->nrhs);
	c->head			= col1;
	c->client_data_t	= _NETW_RULE_T;
	c->client_data		= (void *) r;
	netw__col_append_cell( col1, c );
	crule = cell->left[ir] = c;
	// Allocate for links from rule to LHS and RHS
	crule->nleft = (r->nrhs + r->ngetters);
	crule->left = (netw_cell_rec_ptr *) malloc( (r->nrhs + r->ngetters) * sizeof(netw_cell_rec_ptr) );
	y2 += 1;
	for( i=0; i < r->nrhs; i++ ){
	  _NETW_NEWCELL( c );
	  c->y = y2++;
	  c->head = col2;
	  c->client_data_t = _NETW_STR_T;
	  c->client_data   = (void *) r->rhs[ r->nrhs - 1 - i ];
	  netw__col_append_cell( col2, c );
	  crule->left[i] = c;
	}
	for( i=0; i < r->ngetters; i++ ){
	  _NETW_NEWCELL( c );
	  c->y = y2++;
	  c->head = col2;
	  c->client_data_t = _NETW_SIGN_T;
	  c->client_data   = (void *) ((cond_rec_ptr) r->getters[ r->ngetters - 1 - i ])->sign;
	  netw__col_append_cell( col2, c );
	  crule->left[ r->nrhs + i ] = c;
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


void netw_initfill_all( cdCanvas *canvas, double WORLD_W, double WORLD_H ){
  sign_rec_ptr s, top = (sign_rec_ptr) loadkb_get_allhypos();
  col_rec_ptr col = _NEW_COL;
  col->x	= 1;
  col->next	= NULL;
  netw_cell_rec_ptr cptr;
  short i;
  for( s=top, i=0, cptr = (netw_cell_rec_ptr)col; s; s=s->next  ){
    if( 0 == s->nsetters ){
      // Final hypothesis only
      _NETW_NEWCELL( cptr->next );
      cptr->next->y		= i+1;
      cptr->next->head		= col;
      cptr->next->client_data_t = _NETW_SIGN_T;
      cptr->next->client_data	= (void *)s;
      i += 1;
      cptr=cptr->next;
    }
  }
  // Second pass to adjust height of cells
  int inc = (WORLD_H/CELL_H - i)/2;
  printf("INITFILLALL i=%d, inc=%d\n", i, inc );
  for( cptr=col->first; cptr; cptr=cptr->next ){ cptr->y += inc; }
  //
  cdCanvasSetAttribute( canvas, "USERDATA", (char *) col);
}

void netw_free( cdCanvas *canvas ){
  netw_cell_rec_ptr cell, old_cell;
  col_rec_ptr  old_col, col = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" );
  while( col ){
    cell = col->first;
    while( cell ){
      old_cell = cell;
      cell = cell->next;
      if( old_cell->left )  free( (void *) old_cell->left );
      if( old_cell->right ) free( (void *) old_cell->right );
      free( (void *) old_cell );
    }
    old_col = col;
    col     = col->next;
    free( (void *) old_col );
  }
}


void netw_redrawkb( cdCanvas *canvas, int scale, double WORLD_W, double WORLD_H, unsigned short orientation ){
  col_rec_ptr  col;
  netw_cell_rec_ptr cell;
  short i;
  int xv, yv, p;
  int x0, y0, p0;
  int font_height;
  char buf[_NETW_TEMP_BUFSIZE]={0};

  cdCanvasFont( canvas, "Times", CD_PLAIN, 10 );
  cdCanvasTextAlignment( canvas, CD_SOUTH_EAST );
  cdCanvasGetFontDim( canvas, NULL, &font_height, NULL, NULL );
  col = (col_rec_ptr) cdCanvasGetAttribute( canvas, "USERDATA" ); 
  while( col ){
    cell = col->first;
    while( cell ){
      if( 1 == scale ){
	// What to write
	netw__text( canvas, cell, buf, &p );
	// Where to write it
	switch( cell->client_data_t ){
	case _NETW_SIGN_T:
	case _NETW_STR_T:
	  wdCanvasWorld2Canvas( canvas, (double) (WORLD_W - CELL_W*col->x), (double) (CELL_H*cell->y), &xv, &yv);
	  break;
	case _NETW_RULE_T:
	  wdCanvasWorld2Canvas( canvas, (double) (WORLD_W - CELL_W*col->x - (CELL_W - p)/2), (double) (CELL_H*cell->y), &xv, &yv);
	  break;
	}
	cdCanvasText( canvas, xv, yv, buf );
      }
      else{
	int xmin, xmax, ymin, ymax, xw, yh;
	wdCanvasWorld2Canvas( canvas,
			      (double) (WORLD_W - CELL_W*col->x - CELL_W),
			      (double) (CELL_H*cell->y),
			      &xmin, &ymin );
	wdCanvasWorld2Canvas( canvas,
			      (double) (WORLD_W - CELL_W*col->x),
			      (double) (CELL_H*cell->y + CELL_H),
			      &xmax, &ymax );
	xw = (xmax - xmin)/4; yh = (ymax - ymin)/4;
	cdCanvasRect(canvas, xmin + xw, xmax - xw, ymin + yh , ymax - yh );
      }
      // Draw links
      if( cell->nleft ){
	// From origin
	if( 1 == scale ){
	  switch( cell->client_data_t ){
	  case _NETW_SIGN_T:
	  case _NETW_STR_T:
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W - CELL_W*col->x - p),
				  (double) (CELL_H*cell->y + font_height/2),
				  &xv, &yv);
	    break;
	  case _NETW_RULE_T:
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W - CELL_W*col->x - (CELL_W + p)/2),
				  (double) (CELL_H*cell->y + font_height/2),
				  &xv, &yv);
	    break;
	  }
	}
	else{
	  int xmin, xmax, ymin, ymax, xw, yh;
	  wdCanvasWorld2Canvas( canvas,
				(double) (WORLD_W - CELL_W*col->x - CELL_W),
				(double) (CELL_H*cell->y),
				&xmin, &ymin );
	  wdCanvasWorld2Canvas( canvas,
				(double) (WORLD_W - CELL_W*col->x),
				(double) (CELL_H*cell->y + CELL_H),
				&xmax, &ymax );
	  xw = (xmax - xmin)/4; yh = (ymax - ymin)/4;
	  xv = xmin + xw; yv = ymin + yh + yh;
	}
	// To destinations
	for( i=0; i<cell->nleft; i++ ){
	  if( 1 == scale ){
	  netw__text( canvas, cell->left[i], buf, &p0 );
	  switch( cell->left[i]->client_data_t ){
	  case _NETW_SIGN_T:
	  case _NETW_STR_T:
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W - CELL_W*cell->left[i]->head->x),
				  (double) (CELL_H*cell->left[i]->y + font_height/2),
				  &x0, &y0);
	    break;
	  case _NETW_RULE_T:
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W -
					    CELL_W*cell->left[i]->head->x -
					    (CELL_W - p0)/2 ),
				  (double) (CELL_H*cell->left[i]->y + font_height/2),
				  &x0, &y0);
	    break;
	  }
	  }
	  else{
	    int xmin, xmax, ymin, ymax, xw, yh;
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W - CELL_W*cell->left[i]->head->x - CELL_W),
				  (double) (CELL_H*cell->left[i]->y),
				  &xmin, &ymin );
	    wdCanvasWorld2Canvas( canvas,
				  (double) (WORLD_W - CELL_W*cell->left[i]->head->x),
				  (double) (CELL_H*cell->left[i]->y + CELL_H),
				  &xmax, &ymax );
	    xw = (xmax - xmin)/4; yh = (ymax - ymin)/4;
	    x0 = xmax - xw; y0 = ymin + yh + yh;
	  }
	  cdCanvasLine( canvas, xv, yv, x0, y0 );
	}
      }
      //
      cell = cell->next;
    }
    col = col->next;
  }
}
