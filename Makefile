all: main

main: main.c
	gcc main.c -g -lm -o bmp-gen

clean:
	rm -f bmp-gen
