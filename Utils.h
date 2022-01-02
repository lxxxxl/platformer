#ifndef _UTILS_H
#define _UTILS_H

#include "Tilengine.h"

// check if provided tile is solid
bool IsSolid(uint16_t tileIndex);
// check if tile at provided position is passable
bool isPassable(int x, int y);
#endif