build:
	gcc hello.c -o hello

run: build
	./hello

clean:
	rm -f ./hello
