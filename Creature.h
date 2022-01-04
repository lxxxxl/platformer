#ifndef _CREATURE_H
#define _CREATURE_H

#include <stdlib.h>
#include "Tilengine.h"
#include "Actor.h"
#include "Utils.h"

void CreatureInit (int index, int type);
void CreatureInitAtPos (int index, int type, int x, int y);
void CreatureDeinit (Actor *actor);

#endif