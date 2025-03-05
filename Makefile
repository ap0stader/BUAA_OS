all: check
	gcc -I./src/include ./src/main.c ./src/output.c -o ./out/main

check:
	gcc check.c -c -o check.o

run:
	./out/main

clean:
	rm -rf check.o ./out/main
