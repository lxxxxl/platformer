#include <stdio.h>
#include "Tilengine.h"
#include "Actor.h"
#include "Player.h"
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

/* entry point */
int main (int argc, char *argv[])
{
	int c;
	TLN_Tilemap foreground;
	TLN_Tilemap background;
	TLN_SequencePack sp;
	TLN_Sequence sequence;
	TLN_Palette palette;

	/* setup engine */
	TLN_Init (WIDTH,HEIGHT, MAX_LAYER, ACTOR_ENEMY_MAX, 1);
	TLN_SetLogLevel(TLN_LOG_VERBOSE);
	TLN_SetBGColor (0,128,238);

	/* load resources*/
	TLN_SetLoadPath ("assets");
	foreground = TLN_LoadTilemap ("map.tmx", NULL);
	TLN_SetLayerTilemap (LAYER_FOREGROUND, foreground);

	/* init actors */
	CreateActors(ACTOR_ENEMY_MAX);
	/* init player */
	PlayerInit();

	/* startup display */
	TLN_CreateWindow (NULL, 0);

	/* main loop */
	while (TLN_ProcessWindow ())
	{

		/* process actors tasks */
		TasksActors(frame);

		/* render to window */
		TLN_DrawFrame (0);

		frame++;
	}

	/* deinit */
	TLN_DeleteTilemap (foreground);
	TLN_Deinit ();

	return 0;
}
