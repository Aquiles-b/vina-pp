CFLAGS = -Wall -g

vina: src/vina.c
	gcc src/vina.c -o $@ $(CFLAGS)

rv: vina
	./vina

clean:
	rm vina
