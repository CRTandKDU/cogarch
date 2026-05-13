#include <stdio.h>
#include <stddef.h>
#include "nxp_hash.h"

void cb( char *name, char *prop, char *key, char *val ){
  printf( "CB %s %s %s %s\n", name, prop, key, val );
}

// gcc -Wall -Wextra hello.c ../src/zhash.c
// prints "hello world" to stdout
int main ()
{
  nxp_hash_open();

  nxp_hash_set( "$task", "VALUE", "FLUID_TRANSFER" );
  nxp_hash_set( "$task", "VALUE", "GROUND_TEST" );
  nxp_hash_set( "$task", "VALUE", "CLOSING" );
  nxp_hash_set( "$task", "VALUE", "OTHER" );

  nxp_hash_set( "$task", "SEEALSO", "ALERT" );
  nxp_hash_set( "$task", "SEEALSO", "POSIIBLE_LEAK" );

  nxp_hash_set( "EXC_P_RISE", "SEEALSO", "ALERT" );
  nxp_hash_set( "EXC_P_RISE", "SEEALSO", "POSSIBLE_LEAK" );

  /* nxp_hash_print(); */
  nxp_hash_iterate( "$task", "SEEALSO", cb );

  nxp_hash_close();
  return 0;
}
