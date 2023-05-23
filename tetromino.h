#ifndef TETROMINO_H
#define TETROMINO_H

#include <stdint.h>

typedef uint8_t u8;
typedef int32_t s32;

typedef struct Tetromino {
	const u8 *data;
	const s32 side;
} Tetromino;

const u8 TETROMINO_1[] = {
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0,
};

const u8 TETROMINO_2[] = {
	2,2,
	2,2,
};

const u8 TETROMINO_3[] = {
	0,3,0,
	3,3,3,
	0,0,0,
};

const u8 TETROMINO_4[] = {
	0,4,4,
	4,4,0,
	0,0,0,
};

const u8 TETROMINO_5[] = {
	5,5,0,
	0,5,5,
	0,0,0,
};

const u8 TETROMINO_6[] = {
	6,0,0,
	6,6,6,
	0,0,0,
};

const u8 TETROMINO_7[] = {
	0,0,7,
	7,7,7,
	0,0,0,
};

u8 TETRIS_TEXT[] = {
	// 10 by 18 board
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,8,8,8,0,0,8,8,8,0,
	0,0,8,0,0,0,8,0,8,0,
	0,0,8,0,0,0,8,8,0,0,
	0,0,8,0,0,0,8,0,8,0,
	0,0,0,0,0,0,0,0,0,0,
	0,8,8,8,0,0,0,8,0,0,
	0,8,0,0,0,0,0,8,0,0,
	0,8,8,8,0,0,0,8,0,0,
	0,8,0,0,0,0,0,8,0,0,
	0,8,8,8,0,0,0,0,0,0,
	0,0,0,0,0,0,8,8,8,0,
	0,8,8,8,0,0,8,0,0,0,
	0,0,8,0,0,0,8,8,8,0,
	0,0,8,0,0,0,0,0,8,0,
	0,0,8,0,0,0,8,8,8,0,
};

const Tetromino TETROMINOS[] = {
	{TETROMINO_1,4},
	{TETROMINO_2,2},
	{TETROMINO_3,3},
	{TETROMINO_4,3},
	{TETROMINO_5,3},
	{TETROMINO_6,3},
	{TETROMINO_7,3},
};

u8 tetromino_value(const Tetromino *tetromino, s32 row, s32 col, s32 rotation) {
	s32 side = tetromino->side;
	switch (rotation)
	{
	case 0:
		return tetromino->data[row * side + col];
	case 1:
		return tetromino->data[(side - col - 1) * side + row];
	case 2:
		return tetromino->data[(side - row - 1) * side + (side - col - 1)];
	case 3:
		return tetromino->data[col * side + (side - row - 1)];
	default:
		return 0;
	}
}

#endif 
