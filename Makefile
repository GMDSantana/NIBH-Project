
brain:	brain.c forecast.c
	gcc -o brain brain.c -lm


clean:
	rm -fr brain
	rm -fr *.o
