all: main

main: hashmap.h hashmap.c main.c
	gcc -Wall -g3 main.c hashmap.c -o main

hashmap: hashmap.c
	gcc -Wall -g3 hashmap.c -o hashmap
