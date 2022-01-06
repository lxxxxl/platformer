#include "Utils.h"
#include "Actor.h"

uint16_t solidTilesCount = 27;
uint16_t solidTiles[] = {0, 1, 2, 3, 4, 5, 6, 10, 13, 17, 18, 19, 20, 24, 25, 26, 27, 31, 32, 33, 34, 41, 42, 43, 44, 49, 50};
int xworld = 0;

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
	TLN_GetLayerTile (0, x+xworld, y, &ti);
	if (!ti.empty && IsSolid(ti.index))
        return false;
    return true;
}

void xworldIncrement()
{
    int i;
    int width = TLN_GetLayerWidth(LAYER_FOREGROUND);
    xworld++;
    for (i=1; i<MAX_ENEMIES+1; i++){
	    Actor *enemy = GetActor(i);
		enemy->x--;
        if (enemy->x < 0)
            enemy->x = width;
	}
    TLN_SetLayerPosition (LAYER_FOREGROUND, xworld, 0);
    TLN_SetLayerPosition (LAYER_BACKGROUND, xworld/3, 0);
}

void xworldDecrement()
{
    int i;
    int width = TLN_GetLayerWidth(LAYER_FOREGROUND);
    xworld--;
    for (i=1; i<MAX_ENEMIES+1; i++){
	    Actor *enemy = GetActor(i);
        enemy->x++;
		if (enemy->x > width)
            enemy->x = 0;
	}
    TLN_SetLayerPosition (LAYER_FOREGROUND, xworld, 0);
    TLN_SetLayerPosition (LAYER_BACKGROUND, xworld/3, 0);
    TLN_GetLayerWidth(LAYER_FOREGROUND);
}