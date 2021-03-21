all: run

main: main.c
	gcc ./main.c -o main

run: main Recipes.txt
	./main
