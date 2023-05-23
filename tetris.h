#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>

#include "color.h"
#include "tetromino.h"

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
	s32 offset_row;
	s32 offset_col;
	s32 rotation;
} Piece;

typedef struct GameState{
	u8 board[WIDTH * HEIGHT];
	u8 lines[HEIGHT];

	s32 start_level;
	s32 level;
	s32 line_count;
	u8 pending_line_count;
	s32 score;

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

	s8 diff_left;
	s8 diff_right;
	s8 diff_up;
	s8 diff_down;
	s8 diff_space;
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

bool is_row_filled(u8 *values, s32 width, s32 row);
bool is_row_empty(u8 *values, s32 width, s32 row);
u8 find_lines(u8 *values, s32 width, s32 height, u8 *lines_out);
void clear_lines(u8 *values, s32 width, s32 height, u8 *lines);

bool is_piece_valid(Piece *piece, u8 *board, s32 width, s32 height);
void spawn_piece(GameState *g_state);
void merge_piece(GameState *g_state);
bool soft_drop(GameState *g_state);
void hard_drop(GameState *g_state);

f32 compute_time_to_next_drop(s32 level);
s32 compute_lines_for_next_level(s32 start_level, s32 level);
s32 compute_score(s32 level, s32 line_count);

void fill_rect(SDL_Renderer *renderer, s32 x, s32 y, s32 width, s32 height, Color color, bool filled);
void draw_cell(SDL_Renderer *renderer, s32 row, s32 col, u8 value, s32 offset_x, s32 offset_y, bool filled);
void draw_piece(SDL_Renderer *renderer, Piece *piece, s32 offset_x, s32 offset_y, bool filled);
void draw_board(SDL_Renderer *renderer, u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y);
void draw_intro_text(SDL_Renderer *renderer, u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y);
void draw_helper(SDL_Renderer *renderer, GameState* g_state);

#endif
