CC = gcc
CFLAGS = -g -Wall

# ifeq ($(UNAME), Linux)
# 	CFLAGS += -O2 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lraylib -lglfw -lGL -lopenal -lm -pthread -ldl
# else 
# 	CFLAGS += -O1 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm
# endif

ifeq ($(OS),Windows_NT)
	CFLAGS += -O1 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME), Linux)
		CFLAGS += -O2 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lraylib -lglfw -lGL -lopenal -lm -pthread -ldl
	endif
endif



all: builder

builder: main.o menu.o
	$(CC) main.o menu.o -o builder $(CFLAGS)

main.o: main.c menu.h
	$(CC) $(CFLAGS) -c main.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c

clean:
	rm *.o