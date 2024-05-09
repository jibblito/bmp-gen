all: main2

run:
	./plot-gen sample_series.csv

main: main.c
	gcc main.c plots.c robotarium.c ../obj/* -g -lm -o plot-gen

main2: main2_tasks.c
	gcc main2_tasks.c plots.c robotarium.c ../obj/* -g -lm -o plot-gen


clean:
	rm -f plot-gen
