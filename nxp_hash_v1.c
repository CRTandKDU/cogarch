/**
 * nxp_hash.c -- Extending the NXP Architecture
 *
 * Written on 2026-05-13.
 */
#include <stdio.h>
#include <stdlib.h>
#include "nxp_hash.h"

#include "src/zhash.h"
#include "src/zsorted_hash.h"

typedef struct ZHashEntry *entry_rec_ptr;
typedef struct ZHashTable       *zhash_ptr;
typedef struct ZSortedHashTable *zsortedhash_ptr;

static zsortedhash_ptr  S_BigHash = NULL;

void nxp_hash_print(){
  zsortedhash_ptr bighash = S_BigHash;
  int i;
  zsortedhash_ptr sht_name, sht_key;
  struct ZIterator *iter_name, *iter_key, *iter;

  fprintf( stderr, "BIGHASH %d entries\n", zsorted_hash_count(bighash) );
  i = 0;
  for( iter_name = zcreate_iterator( bighash );
       ziterator_exists( iter_name );
       ziterator_next( iter_name ) ){
    fprintf( stderr, "\t%d\n", i++ );
    sht_name = (zsortedhash_ptr) ziterator_get_val( iter_name );
    for( iter_key = zcreate_iterator( sht_name );
	 ziterator_exists( iter_key );
	 ziterator_next( iter_key ) ){
      sht_key = (zsortedhash_ptr) ziterator_get_val( iter_key );
      for( iter = zcreate_iterator( sht_key ); ziterator_exists(iter); ziterator_next(iter) ){
	printf( "BHash %s %s key=%s val=%s\n",
		ziterator_get_key( iter_name ),
		ziterator_get_key( iter_key ),
		ziterator_get_key( iter ),
		(char *) ziterator_get_val( iter ) );
      }
      zfree_iterator( iter );
    }
    zfree_iterator( iter_key );
  }
  zfree_iterator( iter_name );
}

void nxp_hash__free( zsortedhash_ptr bighash ){
  int i;
  zsortedhash_ptr sht_name, sht_key;
  struct ZIterator *iter_name, *iter_key, *iter;

  // fprintf( stderr, "BIGHASH %d entries\n", zsorted_hash_count(bighash) );
  i = 0;
  for( iter_name = zcreate_iterator( bighash );
       ziterator_exists( iter_name );
       ziterator_next( iter_name ) ){
    // fprintf( stderr, "\t%d\n", i++ );
    sht_name = (zsortedhash_ptr) ziterator_get_val( iter_name );
    for( iter_key = zcreate_iterator( sht_name );
	 ziterator_exists( iter_key );
	 ziterator_next( iter_key ) ){
      sht_key = (zsortedhash_ptr) ziterator_get_val( iter_key );
      for( iter = zcreate_iterator( sht_key ); ziterator_exists(iter); ziterator_next(iter) ){
	// printf( "Clearing %s %s key=%s val=%s\n",
	// 	ziterator_get_key( iter_name ),
	// 	ziterator_get_key( iter_key ),
	// 	ziterator_get_key( iter ),
	// 	(char *) ziterator_get_val( iter ) );
	// MAYBE free val here?
      }
      zfree_iterator( iter );
      zfree_sorted_hash_table( sht_key );
    }
    zfree_iterator( iter_key );
    zfree_sorted_hash_table( sht_name );
  }
  zfree_iterator( iter_name );
  zfree_sorted_hash_table( bighash );
}

void nxp_hash_open(){
  nxp_hash_close();
  //
  S_BigHash = zcreate_sorted_hash_table();
}
  
void nxp_hash_close(){
  if( S_BigHash ) nxp_hash__free( S_BigHash );
}

void nxp_hash_set( char *name, char *key, char *val ){
  if( zsorted_hash_exists( S_BigHash, name ) ){
    // fprintf( stderr, "SET %s found, %s %s\n", name, key, val );
    zsortedhash_ptr sht_name = (zsortedhash_ptr) zsorted_hash_get( S_BigHash, name );
    zsortedhash_ptr sht_key;
    char *skey;
    int  count;
    if( zsorted_hash_exists( sht_name, key ) ){
      // fprintf( stderr, "SET %s found, %s found, %s\n", name, key, val );
      sht_key = (zsortedhash_ptr) zsorted_hash_get( sht_name, key );
      count = (int) zsorted_hash_count( sht_key );
      skey = (char *) malloc( 6*sizeof(char) );
      sprintf( skey, "%d", count + 1 );
      zsorted_hash_set( sht_key, skey, (void *) val );
    }
    else{
      // fprintf( stderr, "SET %s found, %s not found, %s\n", name, key, val );
      // First key, first value
      sht_key = zcreate_sorted_hash_table();
      zsorted_hash_set( sht_key, "1", (void *) val );
      zsorted_hash_set( sht_name, key, (void *) sht_key );
    }
  }
  else{
    // First key, first value for this name
    // fprintf( stderr, "SET %s not found, %s %s\n", name, key, val );
    zsortedhash_ptr sht_name = zcreate_sorted_hash_table();
    zsortedhash_ptr sht_key  = zcreate_sorted_hash_table();
    zsorted_hash_set( sht_key, "1", (void *) val );
    zsorted_hash_set( sht_name, key, (void *) sht_key );
    zsorted_hash_set( S_BigHash, name, (void *) sht_name );
  }
}

void nxp_hash_iterate( char *name, char *key, nxp_hash_iter_t f ){
  if( zsorted_hash_exists( S_BigHash, name ) ){
    zsortedhash_ptr sht_name = (zsortedhash_ptr) zsorted_hash_get( S_BigHash, name );
    if( zsorted_hash_exists( sht_name, key ) ){
      zsortedhash_ptr sht_key = (zsortedhash_ptr) zsorted_hash_get( sht_name, key );
      struct ZIterator *iter;
      for( iter = zcreate_iterator( sht_key ); ziterator_exists(iter); ziterator_next(iter) ){
	f( name, key, ziterator_get_key( iter ), ziterator_get_val( iter ) );
      }
      zfree_iterator( iter );
    }
  }
}
