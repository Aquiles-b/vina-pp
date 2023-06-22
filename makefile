CFLAGS = -Wall -g

vina++: src/main.c src/vina.o src/vina.h
	gcc src/main.c -o vina++ src/vina.o $(CFLAGS)

vina.o: src/vina.c src/vina.h
	gcc -c src/vina.c $(CFLAGS)

clean:
	rm -f src/*.o vina++  
