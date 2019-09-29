#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

//#include <SDL.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;	
typedef float f32;
typedef double f64;

#define WIDTH 10
#define HEIGHT 22
#define VISIBLE_HEIGHT 20

struct Tetromino {
	const u8 *data;
	const s32 side;
};

inline Tetromino tetromino(const u8 *data, s32 side) {
	return { data ,side };
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
	0,0,0,
	3,3,3,
	0,3,0
};

const Tetromino TETROMINOS[] = {
	tetromino(TETROMINO_1,4),
	tetromino(TETROMINO_2,2),
	tetromino(TETROMINO_3,3)
};

enum GamePhase {
	GAME_PHASE_PLAY
};

struct PieceState {
	u8 tetromino_index;
	s32 offset_row;
	s32 offset_col;
	s32 rotation;
};

struct GameState {
	u8 board[WIDTH * HEIGHT];
	PieceState piece;
	GamePhase phase;
};

struct InputState {
	s8 dleft;
	s8 dright;
	s8 dup;
};

inline u8 matrix_get(const u8 *values, s32 width, s32 row, s32 col) {
	s32 index = row * width + col;
	return values[index];
}

inline void matrix_set(u8 *values, s32 width, s32 row, s32 col, u8 value) {
	s32 index = row * width + col;
	values[index] = value;
}

inline u8 tetromino_get(const Tetromino *tetromino, s32 row, s32 col, s32 rotation) {
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

bool check_piece_valid(const PieceState *piece, const u8 *board, s32 width, s32 height) {
	const Tetromino *tetromino = TETROMINOS + piece->tetromino_index;
	assert(tetromino);

	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			u8 value = tetromino_get(tetromino, row, col, piece->rotation);
			if (value > 0) {
				s32 board_row = piece->offset_row + row;
				s32 board_col = piece->offset_col + col;
				if (board_row < 0 || board_row >= height || board_col < 0 || board_col >= height) {
					return false;
				}
				if (matrix_get(board, width, board_row, board_col)) {
					return false;
				}
			}
		}
	}
	return true;
}

void update_gameplay(GameState *game, const InputState *input) {
	PieceState piece = game->piece;
	if (input->dleft > 0) {
		piece.offset_col--;
	}
	if (input->dright > 0) {
		piece.offset_col++;
	}
	if (input->dup > 0) {
		piece.rotation = (piece.rotation + 1) % 4;
	}

	if (check_piece_valid(&piece, game->board, WIDTH, HEIGHT )) {
		game->piece = piece;
	}
}

void update_game(GameState *game, const InputState *input) {
	switch (game->phase) {
	case GAME_PHASE_PLAY:
		return update_gameplay(game, input);
		break;
	}
}

int main()
{
	return 0;
}
