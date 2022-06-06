.PHONY: build

build: client.out

%.out: %.c
	gcc -Wall -o $@ $^

clean:
	rm *.out
