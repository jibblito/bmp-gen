all: main

run:
	./bmp-gen y

objects: colorvec canvas shapes sprites tbox

# gonna have to use root for this my guy
install: objects
	cp ./*.h /usr/local/include/bmp-gen/
	ar rcs /usr/local/lib/libbmp-gen.a obj/*

main: main.c colorvec shapes canvas sprites tbox
	gcc obj/* main.c -g -lm -lX11 -o bmp-gen

main2: main2.c colorvec shapes canvas
	gcc obj/* main2.c -g -lm -o bmp-gen

colorvec: colorvec.c
	gcc colorvec.c -c -g -lm -o obj/colorvec.o

canvas: canvas.c
	gcc canvas.c -c -lx11 -g -o obj/canvas.o

shapes: shapes.c 
	gcc shapes.c -c -g -lm -o obj/shapes.o

sprites: sprites.c 
	gcc sprites.c -c -g -lm -o obj/sprites.o

tbox: tbox.c
	gcc tbox.c -c -g -o obj/tbox.o

clean:
	rm -f bmp-gen
	rm -f obj/*
