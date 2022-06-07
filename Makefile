CC = gcc
CFLAGS = -g -Wall
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CFLAGS += -O2 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lraylib -lglfw -lGL -lopenal -lm -pthread -ldl
else 
	CFLAGS += -O1 -std=c99 -Wno-missing-braces -I include/ -L lib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm
endif

all: builder

builder: main.c
	$(CC) main.c -o builder $(CFLAGS)

clean:
	rm *.o builder