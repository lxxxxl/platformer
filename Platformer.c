#include <stdio.h>
#include "Tilengine.h"
#include "ObjectList.h"
#include "Actor.h"
#include "Creature.h"
#include "Utils.h"

#define WIDTH	640
#define HEIGHT	400

/* layers */
enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND,
	MAX_LAYER
};

unsigned int frame;

void initCreatures()
{
	int c = 1;
	/* init player */
	CreatureInit(ACTOR_PLAYER, TYPE_PLAYER);

	/* get emeny coords from objects layer */
	TLN_ObjectList objectlist = TLN_LoadObjectList("map.tmx", NULL);
	TLN_Object *object = objectlist->list;

	/* place enemies */
	while(object != NULL){
		CreatureInitAtPos(c, TYPE_ENEMY, object->x, object->y-SPRITE_SIZE);
		object = object->next;
		if (c==3)
		break;
		c++;
	}
}

/* entry point */
int main (int argc, char *argv[])
{
	TLN_Tilemap foreground;
	int i, count;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, ACTOR_ENEMY_MAX, 1);
	//TLN_SetLogLevel(TLN_LOG_VERBOSE);
	TLN_SetBGColor (0,128,238);

	/* load resources*/
	TLN_SetLoadPath ("assets");
	foreground = TLN_LoadTilemap ("map.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);

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
		for (i = 1; i < MAX_ENEMIES; i++){
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
