#include <stdio.h>
#include "Tilengine.h"
#include "ObjectList.h"
#include "Actor.h"
#include "Creature.h"
#include "Utils.h"

unsigned int frame;

void initCreatures()
{
	int enemy = 1;
	/* init player */
	CreatureInitAtPos(ACTOR_PLAYER, ACTOR_PLAYER, SCREEN_WIDTH/2, 0);

	/* get emeny coords from objects layer */
	TLN_ObjectList objectlist = TLN_LoadObjectList("map.tmx", NULL);
	TLN_Object *object = objectlist->list;

	/* place enemies */
	while(object != NULL){
		CreatureInitAtPos(enemy, TYPE_ENEMY, object->x, object->y-SPRITE_SIZE);
		object = object->next;
		enemy++;
		if (enemy >= ACTOR_ENEMY_MAX)
			break;
	}
}

/* entry point */
int main (int argc, char *argv[])
{
	TLN_Tilemap foreground;
	TLN_Tilemap background;
	int i, count;

	/* setup engine */
	TLN_Init (SCREEN_WIDTH, SCREEN_HEIGHT, MAX_LAYER, ACTOR_ENEMY_MAX, 1);
	//TLN_SetLogLevel(TLN_LOG_VERBOSE);

	/* load resources*/
	TLN_SetLoadPath ("assets");
	foreground = TLN_LoadTilemap ("map.tmx", "tiles");
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);

	background = TLN_LoadTilemap ("map.tmx", "background");
	TLN_SetLayerTilemap (LAYER_BACKGROUND, background);

	/* init actors */
	CreateActors(ACTOR_ENEMY_MAX);
	initCreatures();

	/* startup display */
	TLN_CreateWindow (NULL, 0);

	/* main loop */
	while (TLN_ProcessWindow ())
	{

		/* process actors tasks */
		TasksActors(frame);

		/* restart game if there is no alive enemies */
		count = 0;
		for (i = 1; i < MAX_ENEMIES+1; i++){
			Actor *enemy = GetActor(i);
			if (enemy->state==ACTIVE)
				count++;
		}
		if (count==0)
			initCreatures();

		/* render to window */
		TLN_DrawFrame (0);

		frame++;
	}

	/* deinit */
	TLN_DeleteTilemap (foreground);
	TLN_Deinit ();

	return 0;
}
