run:
	clang tetris.c -o tetris -W -Wall -Wpedantic -std=c17 -g -fsanitize=address `pkg-config --libs --cflags sdl2`
