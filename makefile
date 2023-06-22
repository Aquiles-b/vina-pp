CFLAGS = -Wall -g

vina++: src/main.c src/trata_entrada.o src/vina.o
	gcc src/main.c -o vina++ src/vina.o src/trata_entrada.o $(CFLAGS)

trata_entrada.o: src/trata_entrada.c src/trata_entrada.h
	gcc -c src/trata_entrada.c  $(CFLAGS)

vina.o: src/vina.c src/vina.h
	gcc -c src/vina.c $(CFLAGS)

clean:
	rm -f src/*.o vina++  
