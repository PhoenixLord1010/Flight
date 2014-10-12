#ifndef _HUD_
#define _HUD_

/*
Functions for handling the HUD and parsing commands
*/

#include "entity.h"

typedef struct
{
  Sprite *sprite;
  Uint32 buttons;
  Uint32 oldbuttons;
  Uint32 state;
  Uint32 shown;
  Uint32 frame;
  Uint16  x, y;
  Uint16 ox,oy;
}Mouse_T;

void DrawHUD();

void DrawStatusBarHoriz(int stat,int range,int FG,int BG,int x, int y, int w, int h);

void InitMouse();
void DrawMouse();
void KillMouse();
Uint32 MouseIn(SDL_Rect rect); /*returns the button mask IF the mouse is in the Rectangle*/
int MouseOver(SDL_Rect rect);

#endif