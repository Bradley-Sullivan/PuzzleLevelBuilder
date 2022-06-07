CC = gcc
CFLAGS = -g -Wall -O1 -std=c99 -Wno-missing-braces -I include/ -L lib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm

all: builder

builder: main.c
	$(CC) main.c -o builder $(CFLAGS)

clean:
	rm *.o builder