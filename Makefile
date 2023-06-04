run:
	clang tetris.c -o tetris -W -Wall -Wpedantic -std=c99 -g -fsanitize=address `pkg-config --libs --cflags sdl2` && ./tetris
