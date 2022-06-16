.PHONY: build

build: clean server.out client.out

%.out: %.c
	gcc -Wall -o $@ $^

clean:
	-rm *.out
