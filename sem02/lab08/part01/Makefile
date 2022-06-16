.PHONY: build clean

build: clean server.out client.out

%.out: %.c
	gcc -o $@ $^

clean:
	-rm *.out
	-rm *.soc
