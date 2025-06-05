/**
 * loadkb.c -- Loading knowledge base files
 *
 * Written on jeudi,  5 juin 2025.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "agenda.h"

const char * _BEG_RULE = "#+BEGIN_RULE";
const char * _END_RULE = "#+END_RULE";
const char * _THEN = "THEN";
const char * _BOOLYES = "YES";
const char * _BOOLNO  = "NO";

#define _DSL_LINE 80
#define _FW(line,pch) pch = strtok( (line), delims );			\
  str_toupper( (pch) );							\
  printf( "[S:%d] FW %s (%d)\n", sno, (pch), strlen((pch)) );		\

#define _TRANSITION(x) 	  printf( "Changing sno from %d to ", sno );    \
  sno		= (x);                                                  \
  printf( "%d\n", sno );                                                \


void str_toupper( char *word ){
   char *s = word;
   while( *s ){
     *s = toupper((unsigned char) *s);
     s++;
   }
}

int loadkb_file( const char *fn ){
  FILE * fp;
  char * line = NULL, *pch;
  size_t len = 0;
  ssize_t read;
  //
  char delims[] = " \t\x0D";
  int lno, sno, retcode, condno, cond_t;
  unsigned short schange;
  char dsl_expr[_DSL_LINE];
  
  fp = fopen( fn, "r" );
  if (fp == NULL)
    return 1;
  retcode       = 0;
  lno		= 1;
  sno		= 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    printf("\nLine %d of length %zu:\n", lno, read);
    printf("%s", line);
    //
    schange	= _TRUE;
    while( schange ){
      schange = _FALSE;
      switch( sno ){
      case 0: // Waiting for beginning of rule, skipping lines
	_FW(line,pch);
	if( 0 == strcmp( _BEG_RULE, pch ) ){
	  pch		= strtok (NULL, delims);
	  printf( "\t[S:%d] FW %s (%d)\n", sno, pch, strlen(pch) );
	  if( NULL != pch && *pch != 0x0A ){
	    printf( "NW %s\n", pch );
	  }
	  else{
	    printf( "NW anonymous rule\n" );
	  }
	  condno = 0;
	  _TRANSITION(1);
	}
	break;
	
      case 1: // Parsing lines as conditions until hypothesis declaration
	strcpy( dsl_expr, line );
	_FW(line,pch);
	//
	if( 0 == strcmp( _THEN, pch ) ){
	  pch		= strtok (NULL, delims);
	  printf( "\t[S:%d] FW %s (%d)\n", sno, pch, strlen(pch) );
	  if( NULL != pch && *pch != 0x0A ){
	    printf( "NW HYPO %s\n", pch );
	  }
	  else{
	    printf( "NW missing hypo\n" );
	    sno = 255;
	    schange = _TRUE;
	    break;
	  }
	  printf( "Rule has %d conds\n", condno );
	  _TRANSITION(2);
	  break;
	}
	//
	condno += 1;
	cond_t = 0;
	if( 0 == strcmp( _BOOLYES, pch ) ) cond_t = 1;
	if( 0 == strcmp( _BOOLNO, pch ) )  cond_t = -1;
	if( 0 == cond_t ){
	  // DSL expression
	  printf( "Line %d DSL: %s\n", lno, dsl_expr );
	}
	else{
	  // Boolean condition NW is sign or hypo
	  pch		= strtok (NULL, delims);
	  printf( "\t[S:%d] FW %s (%d)\n", sno, pch, strlen(pch) );
	  if( NULL != pch && *pch != 0x0A ){
	    printf( "NW HYPO or SIGN %s\n", pch );
	  }
	  else{
	    printf( "NW missing hypo or sign\n" );
	    sno = 255;
	    schange = _TRUE;
	    break;
	  }
	}
	
	break;

      case 2: // Waiting for end of rule
	_FW(line,pch);
	if( 0 == strcmp( _END_RULE, pch ) ){
	  _TRANSITION(0);
	}
	break;

      case 255: // Error in parsing
	printf( "Parsing error at line %d\n", lno );
	retcode = 255;
	goto end_exit;
	break;
      }
    }
    
    lno += 1;
  }

 end_exit:  
  fclose(fp);
  if (line)
    free(line);  

  return retcode;
}
