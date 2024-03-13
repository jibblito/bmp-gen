all: main2

run:
	./bmp-gen y

objects: colorvec canvas shapes

main: main.c obj/colorvec.o obj/shapes.o obj/canvas.o
	gcc obj/* main.c -g -lm -o bmp-gen

main2: main2.c obj/colorvec.o obj/shapes.o obj/canvas.o
	gcc obj/* main2.c -g -lm -o bmp-gen

colorvec: colorvec.c
	gcc colorvec.c -c -g -lm -o obj/colorvec.o

canvas: canvas.c
	gcc canvas.c -c -g -o obj/canvas.o

shapes: shapes.c obj/colorvec.o
	gcc shapes.c obj/colorvec.o -c -g -lm -o obj/shapes.o

clean:
	rm -f bmp-gen
	rm -f obj/*
