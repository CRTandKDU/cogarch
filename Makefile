CC		= g++
CPP		= g++
CFLAGS		= -I. -I../libforth -I../embed-master -I../libcsv -I./tidy_tree/src
FLTK_CFLAGS     = -isystem "C:/Users/chauv/Documents/fltk-1.4.4" -isystem "C:/Users/chauv/Documents/fltk-1.4.4/build"
EXTRA_CFLAGS	= -O2 -Wall -Wextra -std=c99
DSL_CFLAGS	= -D ENGINE_DSL -D ENGINE_DSL_HOWERJFORTH
DSL_LFLAGS      = ../libcsv/libcsv_la-libcsv.o ../embed-master/util.o -L../embed-master -lembed -lm
FLTK_LFLAGS     = -L "C:/Users/chauv/Documents/fltk-1.4.4/build/lib/Debug" -lfltkd
# DSL_CFLAGS	= 
# DSL_LFLAGS      = 
LFLAGS		=  -L../libforth -L../embed-master

DEPS		= agenda.h Makefile
OBJ		= agenda.o sign.o rule.o hypo.o compound.o engine.o engine_dsl.o loadkb.o
APIS		= sign.o rule.o hypo.o compound.o engine.o engine_dsl.o loadkb.o
NETAPIS         = network_draw.o network_edge.o network_node.o network_node_group.o
REPL_DEPS       = Textwindow.hpp Question.hpp Menu.hpp Listview.hpp nxpFListView.hpp Network.hpp

repl: repl.cpp Textwindow.o Question.o Menu.o Listview.o nxpFListView.o $(APIS)
	$(CPP) -o $@ $^ $(CFLAGS) -O3  $(DSL_CFLAGS) $(DSL_LFLAGS) -lfinal

agenda: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(DSL_CFLAGS) $(DSL_LFLAGS)

# network: network.cpp
# 	$(CPP) -o $@ $^ $(CFLAGS) $(FLTK_CFLAGS) -std=c++20 $(FLTK_LFLAGS)

simple_guile: simple_guile.c
	gcc -o simple_guile simple_guile.c `pkg-config --cflags --libs guile-3.0`

simple_rjhforth: simple_rjhforth.c
	gcc $(CFLAGS) -o simple_rjhforth simple_rjhforth.c $(LFLAGS) -lforth

simple_howerjforth: simple_howerjforth.c
	gcc $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $^ ../embed-master/util.o $(LFLAGS) -lembed -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DSL_CFLAGS)

%.o: %.cpp $(REPL_DEPS) $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS) -O3 $(DSL_CFLAGS) 
