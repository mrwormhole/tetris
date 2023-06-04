#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "number.h"
#include "matrix.h"
#include "color.h"
#include "tetromino.h"

#define WIDTH 10
#define HEIGHT 22
#define VISIBLE_HEIGHT 20
#define GRID_SIZE 30
#define INTRO_WIDTH 10
#define INTRO_HEIGHT 18

const u8 FRAMES_PER_DROP[] = { 48,43,38,33,28,23,18,13,8,6,5,5,5,4,4,4,3,3,3,2,2,2,2,2,2,2,2,2,1 };

typedef enum GamePhase {
	GAME_PHASE_START,
	GAME_PHASE_PLAY,
	GAME_PHASE_LINE,
	GAME_PHASE_OVER,
} GamePhase;

typedef struct Piece{
	u8 tetromino_index;
	i32 offset_row;
	i32 offset_col;
	i32 rotation;
} Piece;

typedef struct GameState{
	u8 board[WIDTH * HEIGHT];
	u8 lines[HEIGHT];

	i32 start_level;
	i32 level;
	i32 line_count;
	u8 pending_line_count;
	i32 score;

	f32 next_drop_time;
	f32 highlight_end_time;
	f32 time;

	Piece piece;
	GamePhase phase;
} GameState;

typedef struct InputState{
	u8 left;
	u8 right;
	u8 up;
	u8 down;
	u8 space;

	i8 diff_left;
	i8 diff_right;
	i8 diff_up;
	i8 diff_down;
	i8 diff_space;
} InputState;

typedef struct Game{
	GameState* g_state;
	InputState* in_state;
	SDL_Renderer* renderer;
} Game;

void input(Game *g, bool *quit_out);
void update(Game *g);
void render(Game *g);

void update_gameplay(GameState *g_state, InputState *in_state);
void update_gameline(GameState *g_state);
void update_gamestart(GameState *g_state, InputState *in_state);
void update_gameover(GameState *g_state, InputState *in_state);

bool is_row_filled(u8 *values, i32 width, i32 row);
bool is_row_empty(u8 *values, i32 width, i32 row);
u8 find_lines(u8 *values, i32 width, i32 height, u8 *lines_out);
void clear_lines(u8 *values, i32 width, i32 height, u8 *lines);

bool is_piece_valid(Piece *piece, u8 *board, i32 width, i32 height);
void spawn_piece(GameState *g_state);
void merge_piece(GameState *g_state);
bool soft_drop(GameState *g_state);
void hard_drop(GameState *g_state);

f32 compute_time_to_next_drop(i32 level);
i32 compute_lines_for_next_level(i32 start_level, i32 level);
i32 compute_score(i32 level, i32 line_count);

void fill_rect(SDL_Renderer *renderer, i32 x, i32 y, i32 width, i32 height, Color color, bool filled);
void draw_cell(SDL_Renderer *renderer, i32 row, i32 col, u8 value, i32 offset_x, i32 offset_y, bool filled);
void draw_piece(SDL_Renderer *renderer, Piece *piece, i32 offset_x, i32 offset_y, bool filled);
void draw_board(SDL_Renderer *renderer, u8 *board, i32 width, i32 height, i32 offset_x, i32 offset_y);
void draw_intro_text(SDL_Renderer *renderer, u8 *board, i32 width, i32 height, i32 offset_x, i32 offset_y);
void draw_helper(SDL_Renderer *renderer, GameState* g_state);

#endif
