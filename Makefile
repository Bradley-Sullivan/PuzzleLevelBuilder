CC = gcc
CFLAGS = -g -Wall
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CFLAGS += -O2 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lraylib -lglfw -lGL -lopenal -lm -pthread -ldl
else 
	CFLAGS += -O1 -std=c99 -Wno-missing-braces -I include/ -L lib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm
endif

all: builder

builder: main.o menu.o
	$(CC) main.o menu.o -o builder $(CFLAGS)

main.o: main.c menu.h
	$(CC) $(CFLAGS) -c main.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c

clean:
	rm *.o builder