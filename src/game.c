#include <stdlib.h>
#include "SDL.h"
#include "graphics.h"
#include "entity.h"
#include "level.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer;
extern SDL_Surface *background;
extern SDL_Rect Camera;

void Init_All();


int main(int argc, char *argv[])
{
	int done;
	int keyn;
	Uint8 *keys;
	Entity *player;
	Entity *tile;
	Entity *col;
	Init_All();
	player = MakePlayer();
	RenderLevel(0);
	done = 0;
	do
	{
		UpdateEntities();
		ResetBuffer();
		DrawEntities();
		NextFrame();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(&keyn);
		if(keys[SDLK_ESCAPE])done = 1;
	}while(!done);

	exit(0);
	return 0;
}

void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
}

void Init_All()
{
	Init_Graphics();
	InitSpriteList();
	InitEntityList();
	atexit(CleanUpAll);
}
