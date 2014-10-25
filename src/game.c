#include <stdlib.h>
#include "SDL.h"
#include "graphics.h"
#include "hud.h"
#include "entity.h"
#include "level.h"

extern SDL_Surface *screen;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern SDL_Rect Camera;
extern float offset;

int windowed = 1;

void Init_ALL();
void Update_ALL();
void Draw_ALL();
void StartScreen();

int main(int argc, char *argv[])
{
	int done = 0;
	int keyn;
	Uint8 *keys;
	
	Init_ALL();
	StartScreen();
	do
	{	
		ResetBuffer();
		
		keys = SDL_GetKeyState(&keyn);
		if(keys[SDLK_ESCAPE])done = 1;

		SDL_PumpEvents();

		Draw_ALL();
		Update_ALL();

		NextFrame();
	}while(!done);

	exit(0);
	return 0;
}

void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
}

void Init_ALL()
{
	Init_Graphics(windowed);
	InitSpriteList();
	InitEntityList();
	InitKeyboard();
	InitMessages();
	LoadFonts();
	atexit(CleanUpAll);
}

void Update_ALL()
{
	UpdateEntities();
	UpdateKeyboard();
	UpdateLevel();
}

void Draw_ALL()
{
	DrawEntities();
	DrawMessages();
	DrawHUD();
}

void StartScreen()
{
	int done = 0;
	int delay = 0;
	int keyn;
	Uint8 *keys;
	
	do
	{
		ResetBuffer();
		SDL_PumpEvents();
		
		keys = SDL_GetKeyState(&keyn);
		if(keys[SDLK_RETURN])done = 1;
		
		DrawText("<Flight>", screen, screen->w/2 - 140, screen->h/2.5, IndexColor(White), F_Large);
		if(delay <= 30)
			DrawText("Press Enter", screen, screen->w/2 - 43, screen->h/1.9, IndexColor(White), F_Small);
		if(delay == 0)
			delay = 60;
		else delay--;

		NextFrame();
	}while(!done);

}