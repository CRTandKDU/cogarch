#ifndef NETW_H
#define NETW_H

struct col_rec;
typedef struct col_rec col_rec, *col_rec_ptr;

struct netw_cell_rec;
typedef struct netw_cell_rec netw_cell_rec, *netw_cell_rec_ptr;

struct col_rec {
  short x;
  col_rec_ptr  next;
  netw_cell_rec_ptr first;
};

#define _NEW_COL ((col_rec_ptr) malloc(sizeof( struct col_rec )))

struct netw_cell_rec {
  short y;
  col_rec_ptr  head;
  netw_cell_rec_ptr next;
  void *client_data;
};

#define _NEW_CELL ((netw_cell_rec_ptr) malloc(sizeof( struct netw_cell_rec )))


#define CELL_W 60
#define CELL_H 20

void netw_initfill_all( cdCanvas * );
void netw_free( cdCanvas * );
void netw_redrawkb( cdCanvas *,double, double  );

#endif
