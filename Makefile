ROM ?= roms/ssnake.ch8
main: main.c 
	gcc -Wall -Wextra main.c -o main 

debug: main.c 
	gcc -Wall -Wextra -g -O0 main.c -o main

release: main.c 
	gcc -Wall -Wextra -O3 main.c -o main

run: main
	./main $(ROM)

clean:
	rm -f main
