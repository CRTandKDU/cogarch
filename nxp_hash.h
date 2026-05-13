#ifndef NXP_HASH_H
#define NXP_HASH_H

typedef void (*nxp_hash_iter_t) (char *, char *, char *, char *);

void nxp_hash_open();
void nxp_hash_close();
void nxp_hash_print();
void nxp_hash_set( char *name, char *key, char *val );
void nxp_hash_iterate( char *, char *, nxp_hash_iter_t );

#endif
