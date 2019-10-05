#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include <SDL.h>
#include "Color.h"
#include "Tetromino.h"

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
#define GRID_SIZE 30

const u8 FRAMES_PER_DROP[] = { 48,43,38,33,28,23,18,13,8,6,5,5,5,4,4,4,3,3,3,2,2,2,2,2,2,2,2,2,1 };

enum GamePhase {
	GAME_PHASE_PLAY,
	GAME_PHASE_LINE
};

struct PieceState {
	u8 tetrominoIndex;
	s32 offsetRow;
	s32 offsetCol;
	s32 rotation;
};

struct GameState {
	u8 board[WIDTH * HEIGHT];
	u8 lines[HEIGHT];

	PieceState piece;
	GamePhase phase;

	s32 start_level;
	s32 level;
	s32 line_count;
	u8 pending_line_count;
	s32 score;

	f32 next_drop_time;
	f32 highlight_end_time;
	f32 time;
};

struct InputState {
	u8 left;
	u8 right;
	u8 up;
	u8 down;
	u8 space;

	s8 directionLeft;
	s8 directionRight;
	s8 directionUp;
	s8 directionDown;
	s8 directionSpace;
};

inline u8 getMatrixValue(const u8 *values, s32 width, s32 row, s32 col) {
	s32 index = row * width + col;
	return values[index];
}

inline void setMatrixValue(u8 *values, s32 width, s32 row, s32 col, u8 value) {
	s32 index = row * width + col;
	values[index] = value;
}

inline void printMatrixValues(u8* values, s32 width,s32 height) {
	printf("\n");
	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			s32 index = row * width + col;
			printf("%d ", values[index]);
		}
		printf("\n");
	}
	printf("\n");
}

inline u8 getTetromino(const Tetromino *tetromino, s32 row, s32 col, s32 rotation) {
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

inline bool isRowFilled(const u8 *values, s32 width, s32 row) {
	for (s32 col = 0; col < width; col++) {
		if (!getMatrixValue(values, width, row, col)) {
			return false;
		}
	}
	return true;
}

inline u8 findLines(const u8 *values, s32 width, s32 height, u8 *lines_out) {
	u8 totalFilledRowOfLines = 0;
	for (s32 row = 0; row < height; row++) {
		u8 isFilled = isRowFilled(values,width,row);
		lines_out[row] = isFilled;
		totalFilledRowOfLines += isFilled;
	}
	return totalFilledRowOfLines;
}

void clearLines(u8 *values, s32 width, s32 height, const u8 *lines) {
	s32 src_row = height - 1;
	for (s32 dest_row = height - 1; dest_row >= 0; dest_row--) {
		// check upper blocks
		while (src_row >= 0 && lines[src_row]) {
			src_row--;
		}

		if (src_row < 0) {
			memset(values + dest_row * width, 0, width); // in the end, fulfill the last destination cursor with empty
		}
		else {
			if (src_row != dest_row) {
				memcpy(values + dest_row * width, values + src_row * width, width); // move it from source cursor to destination cursor
			}
			src_row--;
		}
	}
}

bool isPieceValid(const PieceState *piece, const u8 *board, s32 width, s32 height) {
	const Tetromino *tetromino = TETROMINOS + piece->tetrominoIndex;
	assert(tetromino != NULL);

	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, piece->rotation);
			if (value > 0) {
				s32 board_row = piece->offsetRow + row;
				s32 board_col = piece->offsetCol + col;
				if (board_row < 0 || board_row >= height || board_col < 0 || board_col >= width) {
					return false;
				}
				if (getMatrixValue(board, width, board_row, board_col)) {
					return false;
				}
			}
		}
	}
	return true;
}

void mergePiece(GameState *game) {
	const Tetromino *tetromino = TETROMINOS + game->piece.tetrominoIndex;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, game->piece.rotation);
			if (value) {
				s32 board_row = game->piece.offsetRow + row;
				s32 board_col = game->piece.offsetCol + col;
				// we assume everything is in bounds
				setMatrixValue(game->board, WIDTH, board_row, board_col, value);
			}
		}
	}
}

void spawnPiece(GameState *game) {
	game->piece = {};

	game->piece.tetrominoIndex = rand() % 6;
	game->piece.offsetCol = WIDTH / 2 - 1* WIDTH/4;
}

inline f32 get_time_to_next_drop(s32 level) {
	
	if (level > 29) {
		level = 29;
	}

	return FRAMES_PER_DROP[level] * (1.0/60.0f); //FRAMES_PER_DROP * TARGET_SECONDS_PER_FRAME
}

inline s32 get_lines_for_next_level(s32 start_level, s32 level) {
	s32 first_level_up_limit = fmin(start_level * 10 + 10, fmax(100, start_level * 10 - 50));
	if (level == start_level) { return first_level_up_limit; }

	s32 diff = level - start_level;
	return first_level_up_limit + diff * 10;
}

inline bool softDrop(GameState *game) {
	game->piece.offsetRow++;
	if (!isPieceValid(&game->piece, game->board, WIDTH, HEIGHT)) {
		game->piece.offsetRow--;
		mergePiece(game);
		spawnPiece(game);
		return false;
	}

	game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	return true;
}

void hardDrop(GameState *game) {
	bool isPossible = NULL;
	do {
		isPossible = softDrop(game);
	} while (isPossible);
}

inline s32 getScore(s32 level, s32 line_count) {
	switch (line_count) {
	case 1:
		return 40 * (level + 1);
		break;
	case 2:
		return 100 * (level + 1);
		break;
	case 3:
		return 300 * (level + 1);
		break;
	case 4:
		return 1200 * (level + 1);
		break;
	}
	return 0;
}

void updateGameline(GameState *game) {
	if (game->time >= game->highlight_end_time) {
		clearLines(game->board, WIDTH, HEIGHT, game->lines);
		game->line_count += game->pending_line_count;
		game->score += getScore(game->level, game->pending_line_count);
		s32 lines_for_next_level = get_lines_for_next_level(game->start_level, game->level);

		if (game->line_count >= lines_for_next_level) {
			game->level++;
		}

		game->phase = GAME_PHASE_PLAY;
	}
}

void updateGameplay(GameState *game, const InputState *input) {
	PieceState piece = game->piece;
	if (input->directionLeft > 0) {
		piece.offsetCol--;
	}
	if (input->directionRight > 0) {
		piece.offsetCol++;
	}
	if (input->directionUp > 0) {
		piece.rotation = (piece.rotation + 1) % 4;
	}

	if (isPieceValid(&piece, game->board, WIDTH, HEIGHT )) {
		game->piece = piece;
	}

	if (input->directionDown > 0) {
		softDrop(game);
	}

	if (input->directionSpace > 0) {
		hardDrop(game);
	}

	while (game->time > game->next_drop_time) {
		softDrop(game);
	}

	game->pending_line_count = findLines(game->board, WIDTH, HEIGHT, game->lines);
	if (game->pending_line_count > 0) {
		game->phase = GAME_PHASE_LINE;
		game->highlight_end_time = game->time + 0.8f;
	}
	
}

void updateGame(GameState *game, const InputState *input) {
	switch (game->phase) {
	case GAME_PHASE_PLAY:
		updateGameplay(game, input);
		break;
	case GAME_PHASE_LINE:
		updateGameline(game);
		break;
	}
}

void fillRect(SDL_Renderer *renderer, s32 x, s32 y, s32 width, s32 height, Color color) {
	SDL_Rect rect = {x,y,width,height};
	SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
	SDL_RenderFillRect(renderer, &rect);
}

void drawCell(SDL_Renderer *renderer, s32 row, s32 col, u8 value, s32 offset_x, s32 offset_y) {
	Color base_color = BASE_COLORS[value];
	Color light_color = LIGHT_COLORS[value];
	Color dark_color = DARK_COLORS[value];

	s32 edge = GRID_SIZE / 8;
	s32 x = col * GRID_SIZE + offset_x;
	s32 y = row * GRID_SIZE + offset_y;

	fillRect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color);
	fillRect(renderer, x + edge, y, GRID_SIZE -edge, GRID_SIZE -edge, light_color);
	fillRect(renderer, x + edge, y + edge, GRID_SIZE - edge * 2, GRID_SIZE - edge * 2, base_color);
}

void drawPiece(SDL_Renderer *renderer, const PieceState *piece, s32 offset_x, s32 offset_y) {
	const Tetromino *tetromino = TETROMINOS + piece->tetrominoIndex;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, piece->rotation);
			if (value) {
				drawCell(renderer, row + piece->offsetRow, col + piece->offsetCol, value, offset_x, offset_y);
			}
		}
	}
}

void drawBoard(SDL_Renderer *renderer, const u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y) {
	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			u8 value = getMatrixValue(board,width,row,col);
			if (value) {
				drawCell(renderer, row, col, value, offset_x, offset_y);
			}
		}
	}
}

void renderGame(const GameState *game, SDL_Renderer *renderer) {
	drawBoard(renderer, game->board, WIDTH, HEIGHT, 0, 0);
	drawPiece(renderer, &game->piece, 0, 0);
	
	if (game->phase == GAME_PHASE_LINE) {
		for (s32 row = 0; row < HEIGHT; row++) {
			if (game->lines[row])
			{
				s32 x = 0;
				s32 y = row * GRID_SIZE;
				fillRect(renderer, x, y, WIDTH * GRID_SIZE, GRID_SIZE, highlightColor);
			}
		}
	}
}

void registerKeyEvents(InputState *input, bool *quit_out) {
	s32 key_count;
	const Uint8 *keyStates = SDL_GetKeyboardState(&key_count);

	if (keyStates[SDL_SCANCODE_ESCAPE]) { *quit_out = true; }

	InputState previousInput = *input;

	input->left = keyStates[SDL_SCANCODE_LEFT];
	input->right = keyStates[SDL_SCANCODE_RIGHT];
	input->up = keyStates[SDL_SCANCODE_UP];
	input->down = keyStates[SDL_SCANCODE_DOWN];
	input->space = keyStates[SDL_SCANCODE_SPACE];

	input->directionLeft = input->left - previousInput.left;
	input->directionRight = input->right - previousInput.right;
	input->directionUp = input->up - previousInput.up;
	input->directionDown = input->down - previousInput.down;
	input->directionSpace = input->space - previousInput.space;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL library initialization failed");
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 720,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GameState game = {};
	InputState input = {};

	spawnPiece(&game);
	game.piece.tetrominoIndex = 0;

	bool quit = false;
	while (!quit) {
		game.time = SDL_GetTicks() / 1000.0f;

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}

		registerKeyEvents(&input, &quit);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		updateGame(&game, &input);
		renderGame(&game,renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
