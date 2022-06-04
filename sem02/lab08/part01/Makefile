.PHONY: build

build: server.out client.out

%.out: %.c
	gcc -o $@ $^

clean:
	rm *.out
	rm name*
