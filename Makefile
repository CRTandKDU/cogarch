CC = gcc
CFLAGS =  -I. -I../libforth -I../embed-master
EXTRA_CFLAGS = -O2 -Wall -Wextra -std=c99
LFLAGS =  -L../libforth -L../embed-master
DEPS = agenda.h
OBJ = agenda.o sign.o rule.o hypo.o engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

agenda: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

listview: listview.cpp sign.o rule.o hypo.o
	$(CC) -o $@ $^ $(CFLAGS) -O3 -lfinal

simple_guile: simple_guile.c
	gcc -o simple_guile simple_guile.c `pkg-config --cflags --libs guile-3.0`

simple_rjhforth: simple_rjhforth.c
	gcc $(CFLAGS) -o simple_rjhforth simple_rjhforth.c $(LFLAGS) -lforth

simple_howerjforth: simple_howerjforth.c
	gcc $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $^ ../embed-master/util.o $(LFLAGS) -lembed -lm
