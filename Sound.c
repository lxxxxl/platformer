#include "Sound.h"

void initSound()
{
	char *sounds[] = {"assets/jump.wav", "assets/kill.wav"};
	TLN_SoundInit(2, sounds);
}


void playJump()
{
	TLN_PlaySound(0);
}

void playKill()
{
	TLN_PlaySound(1);
}
