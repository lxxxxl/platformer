#include "Creature.h"

typedef enum
{
	CREATURE_IDLE,
	CREATURE_WALKING,
	CREATURE_JUMPING
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
	int sy;	// jump var
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
	creature->sy = 0;
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

	case CREATURE_WALKING:
		TLN_SetSpriteAnimation (actor->index, creature->walk, 0);
		break;

	case CREATURE_JUMPING:
		TLN_DisableSpriteAnimation (actor->index);
		TLN_SetSpritePicture (actor->index, 2);
		creature->sy = -JUMP_HEIGHT;
		break;
	}
}


void CreatureTasks (Actor *actor)
{
	int y2, s0, i;
	Direction input = 0;
	bool jump = false;
	Creature *creature = (Creature*)actor->usrdata;

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
		if (creature->direction==DIR_LEFT){
			if (!isPassable(actor->x+SPRITE_SIZE/2, actor->y+SPRITE_SIZE))
				input = DIR_LEFT;
			else
				input = DIR_RIGHT;
		}
		else{
			if (!isPassable(actor->x+SPRITE_SIZE/2, actor->y+SPRITE_SIZE))
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

	switch (creature->state)
	{
	case CREATURE_IDLE:
		if (input)
			CreatureSetState (actor, CREATURE_WALKING);
		break;

	case CREATURE_WALKING:
	
		if (input == DIR_RIGHT){
			if ((actor->x < TLN_GetWidth()-SPRITE_SIZE) && isPassable(actor->x+SPRITE_SIZE, actor->y))
				actor->x++;
			else if (actor->type!=TYPE_PLAYER)
				creature->direction=DIR_LEFT;	// change ai direction if there is obstacle ahead
		}
		else if (input == DIR_LEFT){
			if ((actor->x > 0)  && isPassable(actor->x-1, actor->y))
				actor->x--;
			else if (actor->type!=TYPE_PLAYER)
				creature->direction=DIR_RIGHT;	// change ai direction if there is obstacle ahead
		}

		if (!input)
			CreatureSetState (actor, CREATURE_IDLE);
		break;

	/* TODO do something when falling after jump right into non-passable tile*/
	case CREATURE_JUMPING:
		if (input == DIR_RIGHT && (actor->x < TLN_GetWidth()-SPRITE_SIZE))
			actor->x++;
		else if (input == DIR_LEFT && (actor->x > 0))
			actor->x--;
		break;

	}

	
	if (jump && creature->state!=CREATURE_JUMPING)
		CreatureSetState (actor, CREATURE_JUMPING);

	/* gravity */
	s0 = creature->sy;
	if (creature->sy < 10)
		creature->sy++;
	y2 = actor->y + (creature->sy>>2);

	//if (actor->type==TYPE_PLAYER)
	//	printf("%d %d %d\n", creature->sy, y2, actor->y);

	if ((creature->sy > 0) && 	// perform this check only when falling
	(y2 % SPRITE_SIZE) <= 1 &&	// ... and only at the top of sprite
	!isPassable(actor->x+SPRITE_SIZE/2, y2+SPRITE_SIZE)){
			creature->sy = 0;
	}

	if (s0>0 && creature->sy==0)
		CreatureSetState (actor, CREATURE_IDLE);

	/* prevent jump out from screen */
	if (y2 < 0)
		y2 = 0;
	actor->y = y2;

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
		if (creature->sy==10){
			Creature *c = (Creature*)enemy->usrdata;
			/* show explosion */
			c->spriteset = TLN_LoadSpriteset("explode");
			c->walk = TLN_CreateSpriteSequence (NULL, c->spriteset, "explode", 2);
			TLN_SetSpriteSet (enemy->index, c->spriteset);
			enemy->state = DEAD;
			SetActorTimeout(enemy, 1, 5);

			creature->sy = -JUMP_HEIGHT/3;
		}

	}

}
