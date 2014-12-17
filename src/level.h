#ifndef __LEVEL__
#define __LEVEL__

/*
Functions for displaying and updating the levels
*/

#include "SDL.h"
#include "graphics.h"

void RenderLevel(int CurrentLevel);
void UpdateLevel();
void LoadMusic(char *bg);

#endif