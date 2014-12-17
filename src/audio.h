#ifndef __Audio__
#define __Audio__

#include "SDL_mixer.h"

typedef struct
{
  Mix_Chunk *sound;
  char filename[30];
  int used;
  int volume;
}Sound;


void Init_Audio();
void InitSoundList();
void ClearSoundList();
void FreeSound(Sound *sound);
Sound *LoadSound(char filename[30],int volume);


#endif