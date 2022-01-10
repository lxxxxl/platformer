#ifndef _UTILS_H
#define _UTILS_H

#include "Tilengine.h"

#define SPRITE_SIZE 16
#define JUMP_HEIGHT 20
#define MAX_ENEMIES 16

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

typedef enum
{
	TILE_NONE,
	TILE_ONEWAY,
	TILE_SOLID,
	TILE_COLLECT
} TileType;

// get tile type at position
// applies xworld
TileType getTileType(int x, int y);
// increment/decrement xworld var and recalculate coords of enemies
void xworldIncrement();
void xworldDecrement();

#endif