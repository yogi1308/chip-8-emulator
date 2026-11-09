ROM ?= roms/snake.ch8
CC = gcc
CFLAGS = -Wall -Wextra -lraylib
SRC = main.c $(wildcard utilities/*.c)
HDRS = $(wildcard *.h) $(wildcard headers/*.h)

main: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) $(SRC) -o main

debug: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -g -O0 $(SRC) -o main

release: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -O3 $(SRC) -o main

run: main
	./main $(ROM)

clean:
	rm -f main

