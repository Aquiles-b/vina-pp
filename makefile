CFLAGS = -Wall -g
OBJECTS = src/trata_entrada.o src/vina.o src/diretorios.o

vina++: src/main.c $(OBJECTS)
	gcc src/main.c -o vina++ $(OBJECTS) $(CFLAGS)

trata_entrada.o: src/trata_entrada.c src/trata_entrada.h
	gcc -c src/trata_entrada.c $(CFLAGS)

vina.o: src/vina.c src/vina.h
	gcc -c src/vina.c $(CFLAGS)

diretorios.o: src/diretorios.c src/diretorios.h
	gcc -c src/diretorios.c $(CFLAGS)

clean:
	rm -f src/*.o vina++  
