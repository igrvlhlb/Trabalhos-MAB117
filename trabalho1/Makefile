CC = gcc
CFLAGS = -lpthread -lm

all: main

simpson.o: simpson.c simpson.h
	${CC} ${CFLAGS} -c $<

main: main.c simpson.o
	${CC} ${CFLAGS} -o $@ $^

stress_test: stress_test.c simpson.o
	${CC} ${CFLAGS} -o $@ $^

test: stress_test
	./stress_test

clean:
	rm simpson.o main stress_test
