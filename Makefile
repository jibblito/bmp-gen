all: main

main: main.c
	gcc main.c ColorVec.c -g -lm -o bmp-gen

mainrun: main.c
	gcc main.c ColorVec.c -g -lm -o bmp-gen
	./bmp-gen kee

clean:
	rm -f bmp-gen
