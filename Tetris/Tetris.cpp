#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

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
	u8 tetromino_index;
	s32 offset_row;
	s32 offset_col;
	s32 rotation;
};

struct GameState {
	u8 board[WIDTH * HEIGHT];
	u8 lines[HEIGHT];

	PieceState piece;
	GamePhase phase;

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
		while (src_row >= 0 && lines[src_row]) {
			src_row--;
		}

		if (src_row < 0) {
			memset(values + dest_row * width, 0, width);
		}
		else {
			if (src_row != dest_row) {
				memcpy(values + dest_row * width, values + src_row * width, width);
			}
			src_row--;
		}
	}
}

bool check_piece_valid(const PieceState *piece, const u8 *board, s32 width, s32 height) {
	const Tetromino *tetromino = TETROMINOS + piece->tetromino_index;
	assert(tetromino != NULL);

	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, piece->rotation);
			if (value > 0) {
				s32 board_row = piece->offset_row + row;
				s32 board_col = piece->offset_col + col;
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

void merge_piece(GameState *game) {
	const Tetromino *tetromino = TETROMINOS + game->piece.tetromino_index;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, game->piece.rotation);
			if (value) {
				s32 board_row = game->piece.offset_row + row;
				s32 board_col = game->piece.offset_col + col;
				// we assume everything is in bounds
				setMatrixValue(game->board, WIDTH, board_row, board_col, value);
			}
		}
	}
}

void spawn_piece(GameState *game) {
	game->piece = {};

	game->piece.tetromino_index = rand() % 6;
	game->piece.offset_col = WIDTH / 2 - 1* WIDTH/4;
}

inline f32 get_time_to_next_drop(s32 level) {
	
	if (level > 29) {
		level = 29;
	}

	return FRAMES_PER_DROP[level] * (1.0/60.0f); //FRAMES_PER_DROP * TARGET_SECONDS_PER_FRAME
}

inline bool soft_drop(GameState *game) {
	game->piece.offset_row++;
	if (!check_piece_valid(&game->piece, game->board, WIDTH, HEIGHT)) {
		game->piece.offset_row--;
		merge_piece(game);
		spawn_piece(game);
		return false;
	}

	game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	return true;
}

void hard_drop(GameState *game) {
	bool isPossible = NULL;
	do {
		isPossible = soft_drop(game);
	} while (isPossible);
}

inline s32 get_score(s32 level, s32 line_count) {
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

void update_gameline(GameState *game) {
	if (game->time >= game->highlight_end_time) {
		clearLines(game->board, WIDTH, HEIGHT, game->lines);
		game->line_count += game->pending_line_count;
		game->score += get_score(game->level, game->pending_line_count);
		game->phase = GAME_PHASE_PLAY;
	}
}

void update_gameplay(GameState *game, const InputState *input) {
	PieceState piece = game->piece;
	if (input->directionLeft > 0) {
		piece.offset_col--;
	}
	if (input->directionRight > 0) {
		piece.offset_col++;
	}
	if (input->directionUp > 0) {
		piece.rotation = (piece.rotation + 1) % 4;
	}

	if (check_piece_valid(&piece, game->board, WIDTH, HEIGHT )) {
		game->piece = piece;
	}

	if (input->directionDown > 0) {
		soft_drop(game);
	}

	if (input->directionSpace > 0) {
		hard_drop(game);
	}

	while (game->time > game->next_drop_time) {
		soft_drop(game);
	}

	game->pending_line_count = findLines(game->board, WIDTH, HEIGHT, game->lines);
	if (game->pending_line_count > 0) {
		game->phase = GAME_PHASE_LINE;
		game->highlight_end_time = game->time + 0.5f;
	}
}

void update_game(GameState *game, const InputState *input) {
	switch (game->phase) {
	case GAME_PHASE_PLAY:
		update_gameplay(game, input);
		break;
	case GAME_PHASE_LINE:
		update_gameline(game);
		break;
	}
}

static bool once = true;

void fill_rect(SDL_Renderer *renderer, s32 x, s32 y, s32 width, s32 height, Color color) {
	SDL_Rect rect = {x,y,width,height};
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void draw_cell(SDL_Renderer *renderer, s32 row, s32 col, u8 value, s32 offset_x, s32 offset_y) {
	Color base_color = BASE_COLORS[value];
	Color light_color = LIGHT_COLORS[value];
	Color dark_color = DARK_COLORS[value];

	s32 edge = GRID_SIZE / 8;
	s32 x = col * GRID_SIZE + offset_x;
	s32 y = row * GRID_SIZE + offset_y;

	fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color);
	fill_rect(renderer, x + edge, y, GRID_SIZE -edge, GRID_SIZE -edge, light_color);
	fill_rect(renderer, x + edge, y + edge, GRID_SIZE - edge * 2, GRID_SIZE - edge * 2, base_color);
}

void draw_piece(SDL_Renderer *renderer, const PieceState *piece, s32 offset_x, s32 offset_y) {
	const Tetromino *tetromino = TETROMINOS + piece->tetromino_index;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = getTetromino(tetromino, row, col, piece->rotation);
			if (value) {
				draw_cell(renderer, row + piece->offset_row, col + piece->offset_col, value, offset_x, offset_y);
			}
		}
	}
}

void draw_board(SDL_Renderer *renderer, const u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y) {
	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			u8 value = getMatrixValue(board,width,row,col);
			if (value) {
				draw_cell(renderer, row, col, value, offset_x, offset_y);
			}
		}
	}
}

void render_game(const GameState *game, SDL_Renderer *renderer) {
	draw_board(renderer, game->board, WIDTH, HEIGHT, 0, 0);
	draw_piece(renderer, &game->piece, 0, 0);
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 720,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GameState game = {};
	InputState input = {};

	spawn_piece(&game);
	game.piece.tetromino_index = 0;

	bool quit = false;
	while (!quit) {
		game.time = SDL_GetTicks() / 1000.0f;

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}

		//check if any key pressed
		s32 key_count;
		const Uint8 *key_states = SDL_GetKeyboardState(&key_count);

		if (key_states[SDL_SCANCODE_ESCAPE]) { quit = true; }

		InputState previous_input = input;

		input.left = key_states[SDL_SCANCODE_LEFT];
		input.right = key_states[SDL_SCANCODE_RIGHT];
		input.up = key_states[SDL_SCANCODE_UP];
		input.down = key_states[SDL_SCANCODE_DOWN];
		input.space = key_states[SDL_SCANCODE_SPACE];

		input.directionLeft = input.left - previous_input.left;
		input.directionRight = input.right - previous_input.right;
		input.directionUp = input.up - previous_input.up;
		input.directionDown = input.down - previous_input.down;
		input.directionSpace = input.space - previous_input.space;
		//check if any key pressed

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		update_game(&game, &input);
		render_game(&game,renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
