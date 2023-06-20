CFLAGS = -Wall -g

main: src/main.c src/vina.o src/vina.h
	gcc src/main.c -o main src/vina.o $(CFLAGS)

vina.o: src/vina.c src/vina.h
	gcc -c src/vina.c $(CFLAGS)

run: main
	./main

clean:
	rm src/*.o main  
