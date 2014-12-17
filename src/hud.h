#ifndef _HUD_
#define _HUD_

/*
Functions for handling the HUD and parsing commands
*/

#include "entity.h"

void DrawHUD();
void DrawStatusBarHoriz(int stat,int range,int FG,int BG,int x, int y, int w, int h);
int ContinueScreen();
int WinScreen();

#endif