all: main

run:
	./plot-gen

main: main.c
	gcc main.c plots.c ../obj/* -g -lm -o plot-gen


clean:
	rm -f plot-gen
