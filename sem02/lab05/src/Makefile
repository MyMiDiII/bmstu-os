run : build
	./testCIO.out
	./testCIOths.out

build : testCIO.c testCIOths.c
	gcc testCIO.c -o testCIO.out
	gcc testCIOths.c -o testCIOths.out

clean :
	rm *.out
