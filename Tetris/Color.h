#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

typedef uint8_t u8;

struct Color
{
	const u8 r;
	const u8 g;
	const u8 b;
	const u8 a;
};

inline Color color(u8 r, u8 g, u8 b, u8 a) {
	return Color{ r,g,b,a };
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

#endif 