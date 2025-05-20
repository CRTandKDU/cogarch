CC = g++
CFLAGS =  -I.
DEPS = agenda.h
OBJ = agenda.o sign.o rule.o hypo.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

agenda: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
