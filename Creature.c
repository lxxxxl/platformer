#include "Creature.h"

typedef enum
{
	CREATURE_IDLE,
	CREATURE_WALK,
	CREATURE_JUMP,
	CREATURE_FALL
}
CreatureState;

typedef enum
{
	DIR_NONE,
	DIR_LEFT,
	DIR_RIGHT,
}
Direction;

typedef struct
{
	TLN_Spriteset spriteset;
	TLN_Sequence walk;
	CreatureState state;
	Direction direction;
	int yspeed;
	bool floor;
}
Creature;


void CreatureTasks (Actor *actor);
void CreatureSetState (Actor *actor, int state);

void CreatureInit (int index, int type)
{
	Actor* actor;
	Creature* creature;
	actor = SetActor(index, type, 0,0, SPRITE_SIZE, SPRITE_SIZE, CreatureTasks);
	creature = (Creature*)actor->usrdata;
	if (type == TYPE_PLAYER)
		creature->spriteset = TLN_LoadSpriteset("Player");
	else {
		char filename[32];
		sprintf(filename, "enemy%02d", (rand()%10)+1);
		creature->spriteset = TLN_LoadSpriteset(filename);
	}


	/* create sequences from sprite names */
	creature->walk = TLN_CreateSpriteSequence (NULL, creature->spriteset, "walk", 2);
	
	TLN_SetSpriteSet (actor->index, creature->spriteset);
	
	CreatureSetState (actor, CREATURE_IDLE);
	creature->direction = DIR_RIGHT;
	creature->yspeed = 0;
	creature->floor = false;
}

void CreatureInitAtPos (int index, int type, int x, int y)
{
	Actor* actor;
	CreatureInit(index, type);
	actor = SetActor(index, type, x, y, SPRITE_SIZE, SPRITE_SIZE, CreatureTasks);
	actor->x = x;
	actor->y = y;
}

void CreatureDeinit (Actor *actor)
{
	Creature *creature = (Creature*)actor->usrdata;
	TLN_DeleteSequence(creature->walk);
	TLN_DeleteSpriteset(creature->spriteset);
	TLN_DisableSprite(actor->index);
}

void CreatureSetState (Actor *actor, int state)
{
	Creature *creature = (Creature*)actor->usrdata;

	if (creature->state == state)
		return;

	creature->state = state;
	switch (creature->state)
	{
	case CREATURE_IDLE:
		TLN_DisableSpriteAnimation (actor->index);
		TLN_SetSpritePicture (actor->index, 0);
		break;

	case CREATURE_WALK:
		TLN_SetSpriteAnimation (actor->index, creature->walk, 0);
		break;

	case CREATURE_JUMP:
		playJump();
		TLN_DisableSpriteAnimation (actor->index);
		TLN_SetSpritePicture (actor->index, 2);
		creature->yspeed = -JUMP_HEIGHT;
		creature->floor = false;
		break;
	}
}


void CreatureTasks (Actor *actor)
{
	int ynew, yspeedold, i;
	Direction input = 0;
	bool jump = false;
	Creature *creature = (Creature*)actor->usrdata;
	TileType tile;

	/* player input */
	if (actor->type==TYPE_PLAYER){
		if (TLN_GetInput (INPUT_LEFT))
			input = DIR_LEFT;
		else if (TLN_GetInput (INPUT_RIGHT))
			input = DIR_RIGHT;
		if (TLN_GetInput (INPUT_A))
			jump = true;
	}
	/* .. or ai processing */
	else{
		tile = getTileType(actor->x+SPRITE_SIZE/2, actor->y+SPRITE_SIZE);
		if (creature->direction==DIR_LEFT){
			if (tile == TILE_SOLID || tile == TILE_ONEWAY)
				input = DIR_LEFT;
			else
				input = DIR_RIGHT;
		}
		else{
			if (tile == TILE_SOLID || tile == TILE_ONEWAY)
				input = DIR_RIGHT;
			else
				input = DIR_LEFT;
		}
	}

	/* direction flags */
	if (input==DIR_RIGHT && creature->direction==DIR_LEFT)
	{
		creature->direction = input;
		TLN_SetSpriteFlags (actor->index, 0);
	}
	if (input==DIR_LEFT && creature->direction==DIR_RIGHT)
	{
		creature->direction = input;
		TLN_SetSpriteFlags (actor->index, FLAG_FLIPX);
	}

	if (jump && creature->floor && creature->state!=CREATURE_JUMP && creature->state!=CREATURE_FALL)
		CreatureSetState (actor, CREATURE_JUMP);


	/* gravity */
	yspeedold = creature->yspeed;
	if (creature->yspeed < 10 && !creature->floor)
		creature->yspeed++;
	ynew = actor->y + (creature->yspeed>>2);
	if (creature->yspeed > 0 && !creature->floor && creature->state!=CREATURE_FALL)
		CreatureSetState (actor, CREATURE_FALL);

	/* prevent jump out from screen */
	if (ynew < 0)
		ynew = 0;

	if (!input && !jump && creature->floor)
		CreatureSetState (actor, CREATURE_IDLE);

	switch (creature->state)
	{
	case CREATURE_IDLE:
		if (input)
			CreatureSetState (actor, CREATURE_WALK);
		break;

	
	case CREATURE_JUMP:
		// todo perform this check  only when falling???
		// and only at the top of sprite
		// TODO check 3 points?
		tile = getTileType(actor->x+SPRITE_SIZE/2, ynew);
		if (tile == TILE_SOLID)
			creature->yspeed = 0;
	case CREATURE_FALL:
	case CREATURE_WALK:

		/* check tile under player */
		if (creature->state != CREATURE_JUMP){
			tile = getTileType(actor->x+SPRITE_SIZE/2, ynew+SPRITE_SIZE);
			if (tile == TILE_SOLID || tile == TILE_ONEWAY){
				creature->yspeed = 0;
				creature->floor = true;

				if (yspeedold>0 && creature->yspeed==0)
					CreatureSetState (actor, CREATURE_IDLE);
			}
			else
				creature->floor = false;
		}

		if (input == DIR_RIGHT){
			tile = getTileType(actor->x+SPRITE_SIZE, actor->y+SPRITE_SIZE/2);
			if (actor->type!=TYPE_PLAYER && (tile == TILE_ONEWAY || tile == TILE_SOLID)){
				creature->direction=DIR_LEFT;	// change ai direction if there is obstacle ahead
				TLN_SetSpriteFlags (actor->index, FLAG_FLIPX);
			}
			if (tile != TILE_SOLID){
				if (actor->type!=TYPE_PLAYER || actor->x < SCREEN_WIDTH/2)	// do not increment xworld if it is ai
					actor->x++;
				else
					if (actor->type==TYPE_PLAYER)
						xworldIncrement();
			}
		}
		else if (input == DIR_LEFT){
			tile = getTileType(actor->x-1, actor->y+SPRITE_SIZE/2);
			if (actor->type!=TYPE_PLAYER && (tile == TILE_ONEWAY || tile == TILE_SOLID)){
				creature->direction=DIR_RIGHT;	// change ai direction if there is obstacle ahead
				TLN_SetSpriteFlags (actor->index, 0);
			}
			if (tile != TILE_SOLID){
				if (actor->type!=TYPE_PLAYER || actor->x > SCREEN_WIDTH/2)	// do not increment xworld if it is ai
					actor->x--;
				else
					if (actor->type==TYPE_PLAYER)
						xworldDecrement();
			}
		}

		
		break;
	}

	actor->y = ynew;

	/* process collisions with enemies*/
	if (actor->type!=TYPE_PLAYER)
		return;

	for (i=1; i<MAX_ENEMIES+1; i++){
		Actor *enemy = GetActor(i);

		/* enemy already killed */
		if (enemy->state==FREE)	
			continue;

		/* free killed enemy */
		if (enemy->state==DEAD){
			if(GetActorTimeout(enemy, 1)){
				CreatureDeinit(enemy);
				ReleaseActor(enemy);
			}
			continue;
		}

		/* check collision */	
		if (!CheckActorCollision(actor, enemy))
			continue;

		/* perform check only when falling */
		if (creature->state == CREATURE_FALL){
			Creature *c = (Creature*)enemy->usrdata;
			/* play sound */
			playKill();
			/* show explosion */
			c->spriteset = TLN_LoadSpriteset("explode");
			c->walk = TLN_CreateSpriteSequence (NULL, c->spriteset, "explode", 2);
			TLN_SetSpriteSet (enemy->index, c->spriteset);
			enemy->state = DEAD;
			SetActorTimeout(enemy, 1, 5);

			creature->yspeed = -JUMP_HEIGHT/3;
		}

	}

}
