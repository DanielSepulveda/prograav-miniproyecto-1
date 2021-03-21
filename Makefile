all: run

main: main.c
	gcc ./main.c -o main -lm

run: main Recipes.txt
	./main
