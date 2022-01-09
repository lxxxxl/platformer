#include "Sound.h"

void initSound()
{
	char *sounds[] = {"jump.wav", "kill.wav"};
	TLN_SoundInit(2, sounds);
	TLN_MusicInit("music.mp3");
}


void playJump()
{
	TLN_PlaySound(0);
}

void playKill()
{
	TLN_PlaySound(1);
}

void playMusic()
{
	TLN_MusicVolume(40);
	TLN_PlayMusic();
}
