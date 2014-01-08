
brain:	brain.o
	gcc -o brain brain.o -lm

brain.o: brain.c
	gcc -c brain.c

forecast.o: forecast.c
	gcc -c forecast.c


clean:
	rm -fr brain
	rm -fr *.o
