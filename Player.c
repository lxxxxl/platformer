#include "Tilengine.h"
#include "Player.h"
#include "Utils.h"


#define SPRITE_SIZE 16
#define JUMP_HEIGHT 20

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALKING,
	PLAYER_JUMPING
}
PlayerState;

typedef enum
{
	DIR_NONE,
	DIR_LEFT,
	DIR_RIGHT,
}
Direction;

TLN_Spriteset player;
TLN_Sequence idle;
TLN_Sequence walk;
TLN_Sequence jump;
TLN_SequencePack sp;

int x,y;
int sy = 0;
PlayerState state;
Direction direction;

void PlayerInit (void)
{
	player = TLN_LoadSpriteset ("Player");
	//sp = TLN_LoadSequencePack ("Player.sqx");

	/* create sequences from sprite names */
	walk = TLN_CreateSpriteSequence (NULL, player, "walk", 2);
	idle = TLN_CreateSpriteSequence (NULL, player, "idle", 2);
	jump = TLN_CreateSpriteSequence (NULL, player, "jump", 2);
	
	TLN_SetSpriteSet (0, player);
	TLN_SetSpritePosition (0, x,y);
	
	PlayerSetState (PLAYER_IDLE);
	direction = DIR_RIGHT;
}

void PlayerDeinit (void)
{
	TLN_DeleteSequence(idle);
	TLN_DeleteSequence(walk);
	TLN_DeleteSequence(jump);
	TLN_DeleteSpriteset (player);
}

void PlayerSetState (int s)
{
	if (state == s)
		return;

	state = s;
	switch (state)
	{
	case PLAYER_IDLE:
		TLN_DisableSpriteAnimation (0);
		TLN_SetSpritePicture (0, 0);
		break;

	case PLAYER_WALKING:
		TLN_SetSpriteAnimation (0, walk, 0);
		break;

	case PLAYER_JUMPING:
		TLN_DisableSpriteAnimation (0);
		TLN_SetSpritePicture (0, 2);
		sy = -JUMP_HEIGHT;
		break;
	}
}


void PlayerTasks (void)
{
	int y2, s0;
	Direction input = 0;
	bool jump = false;

	/* input */
	if (TLN_GetInput (INPUT_LEFT))
		input = DIR_LEFT;
	else if (TLN_GetInput (INPUT_RIGHT))
		input = DIR_RIGHT;
	if (TLN_GetInput (INPUT_A))
		jump = true;

	/* direction flags */
	if (input==DIR_RIGHT && direction==DIR_LEFT)
	{
		direction = input;
		TLN_SetSpriteFlags (0, 0);
	}
	if (input==DIR_LEFT && direction==DIR_RIGHT)
	{
		direction = input;
		TLN_SetSpriteFlags (0, FLAG_FLIPX);
	}

	switch (state)
	{
	case PLAYER_IDLE:
		if (input)
			PlayerSetState (PLAYER_WALKING);
		break;

	case PLAYER_WALKING:
	case PLAYER_JUMPING:
		if (input == DIR_RIGHT){
			if ((x < TLN_GetWidth()-SPRITE_SIZE) && isPassable(x+SPRITE_SIZE, y))
				x++;
		}
		else if (input == DIR_LEFT){
			if ((x > 0)  && isPassable(x-1, y))
				x--;
		}

		if (state==PLAYER_WALKING && !input)
			PlayerSetState (PLAYER_IDLE);
		break;
	}

	
	if (jump && state!=PLAYER_JUMPING &&
		(y - JUMP_HEIGHT - SPRITE_SIZE*2) > 0){	// prevent jump out of screen
		PlayerSetState (PLAYER_JUMPING);
		}

	/* gravity */
	s0 = sy;
	if (sy < 10)
		sy++;
	y2 = y + (sy>>2);

	if ((sy > 0) && 	// perform this check only when falling
	(y2 % SPRITE_SIZE) <= 1 &&	// ... and only at the top of sprite
	!isPassable(x+SPRITE_SIZE/2, y2+SPRITE_SIZE)){
			sy = 0;
	}

	if (s0>0 && sy==0)
		PlayerSetState (PLAYER_IDLE);
	y = y2;

	TLN_SetSpritePosition (0, x,y);
}
