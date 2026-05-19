#ifndef LAYOUT_H
#define LAYOUT_H

typedef void (* layout_enumv_cb_t )( char *, double, double );
typedef void (* layout_enume_cb_t )( char *, double, double,
				     char *, double, double );
typedef void (* layout_update_cb_t)( int, int );

void layout_open( void **, void **, void** );
void layout_close( void *, void *, void *, void* );

void layout_add_edge( void *, void *, void *, char *, char *, int, double );
void layout_run_fr( void *, void **, int, double, double, layout_update_cb_t  );
void layout_run_kk( void *, void **, void *, double, double, layout_update_cb_t );

void layout_enumerate_vertices( void *, void *, void *, layout_enumv_cb_t );
void layout_enumerate_edges( void *, void *, void *, layout_enume_cb_t );

#endif
