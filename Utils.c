#include "Utils.h"
#include "Actor.h"

int xworld = 0;

TileType getTileType(int x, int y)
{
    TLN_TileInfo ti;
	TLN_GetLayerTile (LAYER_FOREGROUND, x+xworld, y, &ti);
    // ti.yoffset != SPRITE_SIZE/2 - case for check when enemy moves
    // ti.yoffset > 2 - case to prevent standing inside of tile
    // TODO fix stand inside of tile when ti.yoffset == SPRITE_SIZE/2
    if (ti.type == TILE_ONEWAY && ti.yoffset != SPRITE_SIZE/2 && ti.yoffset > 1)
        return TILE_NONE;
    return ti.type;
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
}