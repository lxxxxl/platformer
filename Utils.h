#ifndef _UTILS_H
#define _UTILS_H

#include "Tilengine.h"

#define SPRITE_SIZE 16
#define JUMP_HEIGHT 20
#define MAX_ENEMIES 16

// check if provided tile is solid
bool IsSolid(uint16_t tileIndex);
// check if tile at provided position is passable
bool isPassable(int x, int y);
#endif