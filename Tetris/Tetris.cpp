#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

#include <SDL.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;	
typedef float f32;
typedef double f64;

struct Color
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

inline Color color(u8 r, u8 g, u8 b, u8 a) {
	return Color{r,g,b,a};
}

const Color BASE_COLORS[] = {
	color(0x28, 0x28, 0x28, 0xFF),
	color(0x2D, 0x99, 0x99, 0xFF),
	color(0x99, 0x99, 0x2D, 0xFF),
	color(0x99, 0x2D, 0x99, 0xFF),
	color(0x2D, 0x99, 0x51, 0xFF),
	color(0x99, 0x2D, 0x2D, 0xFF),
	color(0x2D, 0x63, 0x99, 0xFF),
	color(0x99, 0x63, 0x2D, 0xFF)
};

const Color LIGHT_COLORS[] = {
	color(0x28, 0x28, 0x28, 0xFF),
	color(0x44, 0xE5, 0xE5, 0xFF),
	color(0xE5, 0xE5, 0x44, 0xFF),
	color(0xE5, 0x44, 0xE5, 0xFF),
	color(0x44, 0xE5, 0x7A, 0xFF),
	color(0xE5, 0x44, 0x44, 0xFF),
	color(0x44, 0x95, 0xE5, 0xFF),
	color(0xE5, 0x95, 0x44, 0xFF)
};

const Color DARK_COLORS[] = {
	color(0x28, 0x28, 0x28, 0xFF),
	color(0x1E, 0x66, 0x66, 0xFF),
	color(0x66, 0x66, 0x1E, 0xFF),
	color(0x66, 0x1E, 0x66, 0xFF),
	color(0x1E, 0x66, 0x36, 0xFF),
	color(0x66, 0x1E, 0x1E, 0xFF),
	color(0x1E, 0x42, 0x66, 0xFF),
	color(0x66, 0x42, 0x1E, 0xFF)
};

#define WIDTH 10
#define HEIGHT 22
#define VISIBLE_HEIGHT 20
#define GRID_SIZE 30

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

void fill_rect(SDL_Renderer *renderer, s32 x, s32 y, s32 width, s32 height, Color color) {
	SDL_Rect rect = {};
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
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
			u8 value = tetromino_get(tetromino, row, col, piece->rotation);
			if (value) {
				draw_cell(renderer, row + piece->offset_row, col + piece->offset_col, value, offset_x, offset_y);
			}
		}
	}
}

void render_game(const GameState *game, SDL_Renderer *renderer) {
	draw_piece(renderer, &game->piece, 0, 0);
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 720,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GameState game = {};
	InputState input = {};

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}


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
