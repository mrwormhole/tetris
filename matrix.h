#ifndef MATRIX_H
#define MATRIX_H

static inline u8 matrix_value(const u8 *values, i32 width, i32 row, i32 col) {
	i32 index = row * width + col;
	return values[index];
}

static inline void set_matrix_value(u8 *values, i32 width, i32 row, i32 col, u8 value) {
	i32 index = row * width + col;
	values[index] = value;
}

static inline void print_matrix(const u8 *values, i32 width,i32 height) {
	printf("\n");
	for (i32 row = 0; row < height; row++) {
		for (i32 col = 0; col < width; col++) {
			i32 index = row * width + col;
			printf("%d ", values[index]);
		}
		printf("\n");
	}
	printf("\n");
}

#endif
