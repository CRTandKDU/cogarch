CC		= gcc
CFLAGS		=  -I. -I../libforth -I../embed-master
EXTRA_CFLAGS	= -O2 -Wall -Wextra -std=c99
DSL_CFLAGS	= -D ENGINE_DSL -D ENGINE_DSL_HOWERJFORTH
DSL_LFLAGS      = ../embed-master/util.o -L../embed-master -lembed -lm
# DSL_CFLAGS	= 
# DSL_LFLAGS      = 
LFLAGS		=  -L../libforth -L../embed-master
DEPS		= agenda.h Makefile
OBJ		= agenda.o sign.o rule.o hypo.o compound.o engine.o engine_dsl.o

agenda: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(DSL_CFLAGS) $(DSL_LFLAGS)

listview: listview.cpp sign.o rule.o hypo.o
	$(CC) -o $@ $^ $(CFLAGS) -O3 -lfinal

simple_guile: simple_guile.c
	gcc -o simple_guile simple_guile.c `pkg-config --cflags --libs guile-3.0`

simple_rjhforth: simple_rjhforth.c
	gcc $(CFLAGS) -o simple_rjhforth simple_rjhforth.c $(LFLAGS) -lforth

simple_howerjforth: simple_howerjforth.c
	gcc $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $^ ../embed-master/util.o $(LFLAGS) -lembed -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DSL_CFLAGS)

