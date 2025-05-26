#include "libforth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

/**
Although multiple instances of a libforth environment can be active in a single
C application, this test program only has one active. This is stored in a 
global variable so signal handlers can access it.
**/
static forth_t *global_forth_environment; 
static int enable_signal_handling;

typedef void (*signal_handler)(int sig); /**< functions for handling signals*/

static int nxp_sign(forth_t *o){
  forth_cell_t val;
  val = forth_pop( o );
  printf( "nxp_sign %d\n", val );
  /* forth_cell_t n; */
  /* (void) forth_string_to_cell( 10, &n, "10" ); */
  /* forth_push( o, n ); */
  return 10;
}

static struct forth_functions *S_nxp_extensions;

static forth_t *forth_initial_enviroment(forth_t **o, forth_cell_t size, 
		FILE *input, FILE *output, enum forth_debug_level verbose, 
		int argc, char **argv)
{
	errno = 0;
	assert(input && output && argv);
	if (*o)
		goto finished;

	S_nxp_extensions = (struct forth_functions *) malloc( sizeof(struct forth_functions) );
	S_nxp_extensions->count = (forth_cell_t) 1;
	S_nxp_extensions->functions = (struct forth_function *) malloc( sizeof(struct forth_function) );
	S_nxp_extensions->functions[0].depth = 1;
	S_nxp_extensions->functions[0].function = &nxp_sign;


	/* *o = forth_init(size, input, output, NULL); */
	*o = forth_init(size, input, output, S_nxp_extensions);

	if (!(*o)) {
		fatal("forth initialization failed, %s", forth_strerror());
		exit(EXIT_FAILURE);
	}

finished:
	forth_set_debug_level(*o, verbose);
	forth_set_args(*o, argc, argv);
	global_forth_environment = *o;
	return *o;
}

/** 
This program can be used as a filter in a Unix pipe chain, or as a standalone
interpreter for Forth. It tries to follow the Unix philosophy and way of
doing things (see <http://www.catb.org/esr/writings/taoup/html/ch01s06.html>
and <https://en.wikipedia.org/wiki/Unix_philosophy>). Whether this is
achieved is a matter of opinion. There are a things this interpreter does
differently to most Forth interpreters that support this philosophy however,
it is silent by default and does not clutter up the output window with "ok",
or by printing a banner at start up (which would contain no useful information
whatsoever). It is simple, and only does one thing (but does it do it well?).
**/
static void fclose_input(FILE **in)
{
	if (*in && (*in != stdin))
		fclose(*in);
	*in = stdin;
}

static int eval_file(forth_t *o, const char *file, enum forth_debug_level verbose) {
	FILE *in = NULL;
	int c = 0, rval = 0;
	assert(file);
	if (verbose >= FORTH_DEBUG_NOTE)
		note("reading from file '%s'", file);
	forth_set_file_input(o, in = forth_fopen_or_die(file, "rb"));
	/* shebang line '#!', core files could also be detected */
	if ((c = fgetc(in)) == '#') 
		while (((c = fgetc(in)) > 0) && (c != '\n'));
	else if (c == EOF)
		goto close;
	else
		ungetc(c, in);
	rval = forth_run(o);
close:	
	fclose_input(&in);
	return rval;
}

/**

To keep things simple options are parsed first then arguments like files,
although some options take arguments immediately after them. 

A library for parsing command line options like *getopt* should be used,
this would reduce the portability of the program. It is not recommended 
that arguments are parsed in this manner.

**/
int main(int argc, char **argv)
{
	FILE *in = NULL, *dump = NULL;
	int rval = 0, i = 1;
       	int save = 0,            /* attempt to save core if true */
	    eval = 0,            /* have we evaluated anything? */
	    readterm = 0,        /* read from standard in */
	    use_line_editor = 0, /* use a line editor, *if* one exists */
	    mset = 0;            /* memory size specified */
	enum forth_debug_level verbose = FORTH_DEBUG_OFF; /* verbosity level */
	static const size_t kbpc = 1024 / sizeof(forth_cell_t); /*kilobytes per cell*/
	static const char *dump_name = "forth.core";
	char *optarg = NULL;
	forth_cell_t core_size = DEFAULT_CORE_SIZE;
	forth_t *o = NULL;
	int orig_argc = argc;
	char **orig_argv = argv;

	/* register_signal_handler(SIGINT, sig_generic_handler); */

#ifdef USE_ABORT_HANDLER
#ifdef __unix__
	register_signal_handler(SIGABRT, sig_abrt_handler);
#endif
#endif

#ifdef _WIN32
	/* unmess up Windows file descriptors: there is a warning about an
	 * implicit declaration of _fileno when compiling under Windows in C99
	 * mode */
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
#endif
/**
JMC Sunday, May 25, 2025
**/
	char str[] = "c\" CRT_and_KDU\" 0 call .s";
	char corefn[] = "forth.fth";
	forth_cell_t val;
	forth_initial_enviroment(&o, core_size, stdin, stdout, verbose, orig_argc, orig_argv);
	if (eval_file(o, corefn, verbose) < 0)
	  goto end;
	if (verbose >= FORTH_DEBUG_NOTE)
	  note("evaluating '%s'", str);
	if (forth_eval(o, str) < 0)
	  goto end;
	val = forth_pop( o );
	printf( "> %d\n", val );
	eval = 1;

end:	
	forth_free(o);
	return rval;
}
