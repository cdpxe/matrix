CFILES=matrix.c
CFLAGS=-W -Wall

all : matrix

matrix : $(CFILES)
	gcc $(CFLAGS) -o matrix $(CFILES) -lncurses

clean :
	rm -fv matrix
