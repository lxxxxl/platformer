#include "Utils.h"

uint16_t solidTilesCount = 27;
uint16_t solidTiles[] = {0, 1, 2, 3, 4, 5, 6, 10, 13, 17, 18, 19, 20, 24, 25, 26, 27, 31, 32, 33, 34, 41, 42, 43, 44, 49, 50};

bool IsSolid(uint16_t tileIndex)
{
    int i;
    for (i = 0; i < solidTilesCount; i++){
        if (solidTiles[i] == tileIndex)
            return true;
    }

    return false;
}

bool isPassable(int x, int y)
{
    TLN_TileInfo ti;
	TLN_GetLayerTile (0, x, y, &ti);
	if (ti.index && IsSolid(ti.index))
        return false;
    return true;
}