#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "matrix.h"
#include "tetris.h"

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL library initialization failed");
		return 1;
	}
	srand(rand());
	SDL_Window* window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 720,  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	GameState g_state = {0};
	InputState in_state = {0};
	Game g = {&g_state, &in_state, renderer};

	spawn_piece(g.g_state);
	g_state.piece.tetromino_index = 0;

	SDL_Event event;
	bool quit = false;
	while (!quit) {
		g_state.time = SDL_GetTicks() / 1000.0f;

		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		
		input(&g, &quit);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		update(&g);
		render(&g);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void input(Game *g, bool *quit_out) {
	s32 key_count;
	const Uint8 *keyStates = SDL_GetKeyboardState(&key_count);

	if (keyStates[SDL_SCANCODE_ESCAPE]) { *quit_out = true; }

	InputState previous_input = *(g->in_state);
	InputState* current = g->in_state;

	current->left = keyStates[SDL_SCANCODE_LEFT];
	current->right = keyStates[SDL_SCANCODE_RIGHT];
	current->up = keyStates[SDL_SCANCODE_UP];
	current->down = keyStates[SDL_SCANCODE_DOWN];
	current->space = keyStates[SDL_SCANCODE_SPACE];

	current->diff_left = current->left - previous_input.left;
	current->diff_right = current->right - previous_input.right;
	current->diff_up = current->up - previous_input.up;
	current->diff_down = current->down - previous_input.down;
	current->diff_space = current->space - previous_input.space;
}

void update(Game *g) {
	GameState* g_state = g->g_state;
	InputState* in_state = g->in_state;

	switch (g_state->phase) {
	case GAME_PHASE_PLAY:
		update_gameplay(g_state, in_state);
		break;
	case GAME_PHASE_LINE:
		update_gameline(g_state);
		break;
	case GAME_PHASE_START:
		update_gamestart(g_state, in_state);
		break;
	case GAME_PHASE_OVER:
		update_gameover(g_state, in_state);
		break;
	}
}

void render(Game *g) {
	GameState* g_state = g->g_state;
	GamePhase g_phase = g_state->phase;
	SDL_Renderer* renderer = g->renderer;

	if (g_phase == GAME_PHASE_PLAY || g_phase == GAME_PHASE_LINE) {
		Piece* p_state = &(g_state->piece);
		draw_board(renderer, g_state->board, WIDTH, HEIGHT, 0, 0);
		draw_piece(renderer, p_state, 0, 0, true);
		draw_helper(renderer, g_state);
	}
	if (g_phase == GAME_PHASE_START) {
		draw_intro_text(renderer, TETRIS_TEXT, INTRO_WIDTH, INTRO_HEIGHT, 0, 0);
	}
	if (g_phase == GAME_PHASE_LINE) {
		for (s32 row = 0; row < HEIGHT; row++) {
			if (g_state->lines[row]) {
				s32 x = 0;
				s32 y = row * GRID_SIZE;
				fill_rect(renderer, x, y, WIDTH * GRID_SIZE, GRID_SIZE, HIGHLIGHT_COLOR, true);
				fill_rect(renderer, x, y, WIDTH* GRID_SIZE, GRID_SIZE, WHITE_COLOR, false);
			}
		}
	}
	if (g_phase == GAME_PHASE_OVER) {
		// gg show the score on screen
	}
}

void update_gameplay(GameState *g_state, InputState *in_state) {
	Piece piece = g_state->piece;

	if (in_state->diff_left > 0) {
		piece.offset_col--;
	}
	if (in_state->diff_right > 0) {
		piece.offset_col++;
	}
	if (in_state->diff_up > 0) {
		piece.rotation = (piece.rotation + 1) % 4;
	}

	if (is_piece_valid(&piece, g_state->board, WIDTH, HEIGHT )) {
		g_state->piece = piece;
	}

	if (in_state->diff_down > 0) {
		soft_drop(g_state);
	}

	if (in_state->diff_space > 0) {
		hard_drop(g_state);
	}

	while (g_state->time > g_state->next_drop_time) {
		soft_drop(g_state);
	}

	g_state->pending_line_count = find_lines(g_state->board, WIDTH, HEIGHT, g_state->lines);
	if (g_state->pending_line_count > 0) {
		g_state->phase = GAME_PHASE_LINE;
		g_state->highlight_end_time = g_state->time + 0.6f;
	}

	s32 game_over_row = fmax(0, HEIGHT - VISIBLE_HEIGHT - 1);
	if (is_row_empty(g_state->board, WIDTH, game_over_row)) {
		g_state->phase = GAME_PHASE_OVER;
	}	
}

void update_gameline(GameState *g_state) {
	if (g_state->time >= g_state->highlight_end_time) {
		clear_lines(g_state->board, WIDTH, HEIGHT, g_state->lines);
		g_state->line_count += g_state->pending_line_count;
		g_state->score += compute_score(g_state->level, g_state->pending_line_count);
		printf("Your score is %d now!! \n", g_state->score);
		if (g_state->line_count >= compute_lines_for_next_level(g_state->start_level, g_state->level)) {
			g_state->level++;
			printf("Current level is %d now!! \n", g_state->level);
		}
		g_state->phase = GAME_PHASE_PLAY;
	}
}

void update_gamestart(GameState *g_state, InputState *in_state) {
	if (in_state->diff_up > 0) {
		g_state->start_level++;
	}
	if (in_state->diff_down > 0 && g_state->start_level > 0) {
		g_state->start_level--;
	}
	if (in_state->diff_space > 0) {
		memset(g_state->board, 0, WIDTH * HEIGHT);
		g_state->level = g_state->start_level;
		g_state->line_count = 0;
		g_state->score = 0;
		spawn_piece(g_state);
		g_state->phase = GAME_PHASE_PLAY;
	}
}

void update_gameover(GameState *g_state, InputState *in_state) {
	if (in_state->diff_space > 0) {
		g_state->phase = GAME_PHASE_START;
	}
}

bool is_row_empty(u8 *values, s32 width, s32 row) {
	// checks if row has any cell, if yes return true if not returns false
	for (s32 col = 0; col < width; col++) {
		if (matrix_value(values, width, row, col)) {
			return true;
		}
	}
	return false;
}

bool is_row_filled(u8 *values, s32 width, s32 row) {
	// checks if row has a gap, if yes return false, if not returns true
	for (s32 col = 0; col < width; col++) {
		if (!matrix_value(values, width, row, col)) {
			return false;
		}
	}
	return true;
}

u8 find_lines(u8 *values, s32 width, s32 height, u8 *lines_out) {
	u8 totalFilledRowOfLines = 0;
	for (s32 row = 0; row < height; row++) {
		u8 isFilled = is_row_filled(values,width,row);
		lines_out[row] = isFilled;
		totalFilledRowOfLines += isFilled;
	}
	return totalFilledRowOfLines;
}

void clear_lines(u8 *values, s32 width, s32 height, u8 *lines) {
	s32 src_row = height - 1;
	for (s32 dest_row = height - 1; dest_row >= 0; dest_row--) {
		// check upper blocks
		while (src_row >= 0 && lines[src_row]) {
			src_row--;
		}

		if (src_row < 0) {
			// in the end, fulfill the last destination cursor with empty
			memset(values + dest_row * width, 0, width); 
		}
		else {
			if (src_row != dest_row) {
				// move it from source cursor to destination cursor
				memcpy(values + dest_row * width, values + src_row * width, width); 
			}
			src_row--;
		}
	}
}

bool is_piece_valid(Piece *piece, u8 *board, s32 width, s32 height) {
	const Tetromino *tetromino = TETROMINOS + piece->tetromino_index;

	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = tetromino_value(tetromino, row, col, piece->rotation);
			if (value > 0) {
				s32 board_row = piece->offset_row + row;
				s32 board_col = piece->offset_col + col;
				if (board_row < 0 || board_row >= height || board_col < 0 || board_col >= width) {
					return false;
				}
				if (matrix_value(board, width, board_row, board_col)) {
					return false;
				}
			}
		}
	}
	return true;
}

void spawn_piece(GameState *g_state) {
	g_state->piece = (Piece){0};
	g_state->piece.tetromino_index = rand() % 7;
	g_state->piece.offset_col = WIDTH / 2 - 1* WIDTH/4;
	g_state->next_drop_time = g_state->time + compute_time_to_next_drop(g_state->level);
}

void merge_piece(GameState *g_state) {
	const Tetromino *tetromino = TETROMINOS + g_state->piece.tetromino_index;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = tetromino_value(tetromino, row, col, g_state->piece.rotation);
			if (value) {
				s32 board_row = g_state->piece.offset_row + row;
				s32 board_col = g_state->piece.offset_col + col;
				// we assume everything is in bounds
				set_matrix_value(g_state->board, WIDTH, board_row, board_col, value);
			}
		}
	}
}

bool soft_drop(GameState *g_state) {
	g_state->piece.offset_row++;
	if (!is_piece_valid(&g_state->piece, g_state->board, WIDTH, HEIGHT)) {
		g_state->piece.offset_row--;
		merge_piece(g_state);
		spawn_piece(g_state);
		return false;
	}

	g_state->next_drop_time = g_state->time + compute_time_to_next_drop(g_state->level);
	return true;
}

void hard_drop(GameState *g_state) {
	bool is_possible = false;
	do {
		is_possible = soft_drop(g_state);
	} while (is_possible);
}

f32 compute_time_to_next_drop(s32 level) {
	if (level > 29) {
		level = 29;
	}

	return FRAMES_PER_DROP[level] * (1.0 / 60.0f); // FRAMES_PER_DROP * TARGET_SECONDS_PER_FRAME
}

s32 compute_lines_for_next_level(s32 start_level, s32 level) {
	s32 first_level_up_limit = fmin(start_level * 10 + 10, fmax(100, start_level * 10 - 50));
	if (level == start_level) { 
		return first_level_up_limit; 
	}

	s32 diff = level - start_level;
	return first_level_up_limit + diff * 10;
}

s32 compute_score(s32 level, s32 line_count) {
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

void fill_rect(SDL_Renderer *renderer, s32 x, s32 y, s32 width, s32 height, Color color, bool filled) {
	SDL_Rect rect = {x,y,width,height};
	SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
	if (filled) {
		SDL_RenderFillRect(renderer, &rect);
	}
	else {
		SDL_RenderDrawRect(renderer, &rect);
	}
}

void draw_cell(SDL_Renderer *renderer, s32 row, s32 col, u8 value, s32 offset_x, s32 offset_y, bool filled) {
	if (value != 8) {
		Color base_color = BASE_COLORS[value];
		Color light_color = LIGHT_COLORS[value];
		Color dark_color = DARK_COLORS[value];

		s32 edge = GRID_SIZE / 8;
		s32 x = col * GRID_SIZE + offset_x;
		s32 y = row * GRID_SIZE + offset_y;

		if (!filled) {
			fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, base_color, false);
		}
		else {
			fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color, true);
			fill_rect(renderer, x + edge, y, GRID_SIZE - edge, GRID_SIZE - edge, light_color, true);
			fill_rect(renderer, x + edge, y + edge, GRID_SIZE - edge * 2, GRID_SIZE - edge * 2, base_color, true);
		}
	}
	else {
		s32 x = col * GRID_SIZE + offset_x;
		s32 y = row * GRID_SIZE + offset_y;
		fill_rect(renderer, x, y, 7 *GRID_SIZE / 8, 7 *GRID_SIZE / 8, CLASSIC_COLORS[rand() % 6], true);
	}
	
}

void draw_piece(SDL_Renderer *renderer, Piece *piece, s32 offset_x, s32 offset_y, bool filled) {
	const Tetromino *tetromino = TETROMINOS + piece->tetromino_index;
	for (s32 row = 0; row < tetromino->side; row++) {
		for (s32 col = 0; col < tetromino->side; col++) {
			u8 value = tetromino_value(tetromino, row, col, piece->rotation);
			if (value) {
				draw_cell(renderer, row + piece->offset_row, col + piece->offset_col, value, offset_x, offset_y, filled);
			}
		}
	}
}

void draw_board(SDL_Renderer *renderer, u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y) {
	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			u8 value = matrix_value(board,width,row,col);
			if (value) {
				draw_cell(renderer, row, col, value, offset_x, offset_y, true);
			}
		}
	}
}

void draw_intro_text(SDL_Renderer *renderer, u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y) {
	for (s32 row = 0; row < height; row++) {
		for (s32 col = 0; col < width; col++) {
			u8 value = matrix_value(board, width, row, col);
			if (value) {
				draw_cell(renderer, row, col, value, offset_x, offset_y, true);
			}
		}
	}
}

void draw_helper(SDL_Renderer *renderer, GameState* g_state) {
	Piece temp = g_state->piece;
	while (is_piece_valid(&temp, g_state->board, WIDTH, HEIGHT)) {
		temp.offset_row++;
	}
	temp.offset_row--;
	draw_piece(renderer, &temp, 0, 0, false);
}



