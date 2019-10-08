#ifndef TETROMINO_H
#define TETROMINO_H

#include <cstdint>

typedef uint8_t u8;
typedef int32_t s32;

struct Tetromino {
	const u8 *data;
	const s32 side;
};

inline Tetromino tetromino(const u8 *data, s32 side) {
	return Tetromino{ data ,side };
}

const u8 TETROMINO_1[] = {
	0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0
};

const u8 TETROMINO_2[] = {
	2,2,
	2,2
};

const u8 TETROMINO_3[] = {
	0,3,0,
	3,3,3,
	0,0,0
};

const u8 TETROMINO_4[] = {
	0,4,4,
	4,4,0,
	0,0,0
};

const u8 TETROMINO_5[] = {
	5,5,0,
	0,5,5,
	0,0,0
};

const u8 TETROMINO_6[] = {
	6,0,0,
	6,6,6,
	0,0,0
};

const u8 TETROMINO_7[] = {
	0,0,7,
	7,7,7,
	0,0,0
};

const u8 TETROMINO_TETRIS[] = {
	0,0,0,0,0,0,0,0,
	8,8,8,0,8,8,8,0,
	0,8,0,0,8,8,0,0,
	0,8,0,0,8,8,0,0,
	0,0,0,0,8,0,8,0,
	8,8,8,0,0,0,0,0,
	8,8,0,0,0,8,0,0,
	8,8,8,0,0,8,0,0,
	0,0,0,0,0,8,0,0,
	8,8,8,0,0,0,0,0,
	0,8,0,0,8,8,8,0,
	0,8,0,0,8,0,0,0,
	0,0,0,0,8,8,8,0,
	0,0,0,0,0,0,8,0,
	0,0,0,0,8,8,8,0
};

const u8 TETROMINO_RIS[] = {
	0,0,0,0,
	8,8,8,0,
	8,0,8,0,
	8,8,0,0,
	8,0,8,0,
	0,0,0,0,
	0,8,0,0,
	0,8,0,0,
	0,8,0,0,
	0,0,0,0,
	8,8,8,0,
	8,0,0,0,
	8,8,8,0,
	0,0,8,0,
	8,8,8,0
};

const Tetromino TETROMINOS[] = {
	tetromino(TETROMINO_1,4),
	tetromino(TETROMINO_2,2),
	tetromino(TETROMINO_3,3),
	tetromino(TETROMINO_4,3),
	tetromino(TETROMINO_5,3),
	tetromino(TETROMINO_6,3),
	tetromino(TETROMINO_7,3),
	tetromino(TETROMINO_TETRIS,15)
};

#endif 

