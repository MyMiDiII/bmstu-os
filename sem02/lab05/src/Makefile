run : build
	./testCIO.out
	./testCIOths.out
	./testKernelIO.out
	./testKernelIOths.out

build : testCIO.c testCIOths.c testKernelIO.c testKernelIOths.c
	gcc testCIO.c -o testCIO.out
	gcc testCIOths.c -o testCIOths.out
	gcc testKernelIO.c -o testKernelIO.out
	gcc testKernelIOths.c -o testKernelIOths.out

clean :
	rm *.out
