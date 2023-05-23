#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

typedef uint8_t u8;
typedef int32_t s32;

u8 matrix_value(const u8 *values, s32 width, s32 row, s32 col);
void set_matrix_value(u8 *values, s32 width, s32 row, s32 col, u8 value);
void print_matrix(const u8 *values, s32 width,s32 height);

u8 matrix_value(const u8 *values, s32 width, s32 row, s32 col) {
	s32 index = row * width + col;
	return values[index];
}

void set_matrix_value(u8 *values, s32 width, s32 row, s32 col, u8 value) {
	s32 index = row * width + col;
	values[index] = value;
}

void print_matrix(const u8 *values, s32 width,s32 height) {
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

#endif
