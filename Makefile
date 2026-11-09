ROM ?= "roms/danm8ku.ch8"
CC = gcc
CFLAGS = -Wall -Wextra
SRC = main.c $(wildcard utilities/*.c)
HDRS = $(wildcard *.h) $(wildcard headers/*.h)
LDLIBS = -lraylib

main: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) $(SRC) -o main $(LDLIBS)

debug: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -g -O0 $(SRC) -o main $(LDLIBS)

release: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -O3 $(SRC) -o main $(LDLIBS)

run: main
	./main $(ROM) --shift-uses-vy --inc-i --speed 500

clean:
	rm -f main

