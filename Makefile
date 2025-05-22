CC = gcc
CFLAGS =  -I.
DEPS = agenda.h
OBJ = agenda.o sign.o rule.o hypo.o engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

agenda: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

listview: listview.cpp sign.o rule.o hypo.o
	$(CC) -o $@ $^ $(CFLAGS) -O3 -lfinal
