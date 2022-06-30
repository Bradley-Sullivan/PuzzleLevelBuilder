CC = gcc
CFLAGS = -g -Wall

ifeq ($(OS),Windows_NT)
	CFLAGS += -O1 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME), Linux)
		CFLAGS += -O2 -std=c99 -Wno-missing-braces -Iinclude/ -Llib/ -lraylib -lglfw -lGL -lopenal -lm -pthread -ldl
	endif
endif



all: builder

builder: main.o menu.o edit_ui.o draw.o
	$(CC) main.o menu.o edit_ui.o draw.o -o builder $(CFLAGS)

main.o: main.c menu.h edit_ui.h draw.h
	$(CC) $(CFLAGS) -c main.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c

edit_ui.o: edit_ui.c edit_ui.h
	$(CC) $(CFLAGS) -c edit_ui.c

draw.o: draw.c draw.h
	$(CC) $(CFLAGS) -c draw.c

clean:
	rm *.o