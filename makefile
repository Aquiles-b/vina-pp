CFLAGS = -Wall -g

vina: src/vina.c
	gcc src/vina.c -o $@ $(CFLAGS)

run: vina
	./vina

clean:
	rm vina archive.vpp
