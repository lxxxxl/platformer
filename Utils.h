#ifndef _UTILS_H
#define _UTILS_H

#include "Tilengine.h"

#define SPRITE_SIZE 16
#define JUMP_HEIGHT 20
#define MAX_ENEMIES 2

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

// check if provided tile is solid
bool IsSolid(uint16_t tileIndex);
// check if tile at provided position is passable
// applies xworld
bool isPassable(int x, int y);
// increment/decrement xworld var and recalculate coords of enemies
void xworldIncrement();
void xworldDecrement();

#endif