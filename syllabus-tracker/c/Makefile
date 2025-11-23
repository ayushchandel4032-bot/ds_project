CC = cc
CFLAGS = -O2 -Wall -Wextra -std=c11

all: syllabus

syllabus: syllabus.o datastructures.o
	$(CC) $(CFLAGS) -o syllabus syllabus.o datastructures.o

syllabus.o: syllabus.c datastructures.h
	$(CC) $(CFLAGS) -c syllabus.c

datastructures.o: datastructures.c datastructures.h
	$(CC) $(CFLAGS) -c datastructures.c

clean:
	rm -f *.o syllabus

