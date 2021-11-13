main: main.c timer.c timer.h
	gcc -O3 -o main main.c timer.c -I.

clean:
	\rm -f *.o main *~ *#
