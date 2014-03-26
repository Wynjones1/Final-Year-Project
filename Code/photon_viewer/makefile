CFLAGS := $(shell sdl2-config --cflags) -O0 -g --std=c11
LIBS   := $(shell sdl2-config --libs) -lGL -lGLU
CC     := gcc

all: main.c reader.c opengl.c
	$(CC) $^ $(CFLAGS) $(LIBS) -o out

run: all
	./out
