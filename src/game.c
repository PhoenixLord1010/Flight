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

int windowed = 1;

void Init_ALL();
void Update_ALL();
void Draw_ALL();

int main(int argc, char *argv[])
{
	int done = 0;
	int keyn;
	Uint8 *keys;
	Entity *player;
	
	Init_ALL();
	player = MakePlayer();
	RenderLevel(0);
	do
	{	
		if(player->sx >= (screen->w * 0.6) + screen->offset)screen->offset += player->sx - ((screen->w * 0.6) + screen->offset);
		//if(player->sx <= (screen->w * 0.2) + screen->offset && screen->offset > 100)screen->offset -= ((screen->w * 0.2) + screen->offset) - player->sx;
		
		ResetBuffer();
		SDL_PumpEvents();

		Draw_ALL();
		Update_ALL();

		NextFrame();
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

void Init_ALL()
{
	Init_Graphics(windowed);
	InitSpriteList();
	InitEntityList();
	InitKeyboard();
	atexit(CleanUpAll);
}

void Update_ALL()
{
	UpdateEntities();
	UpdateKeyboard();
	UpdateLevel(0);
}

void Draw_ALL()
{
	DrawEntities();
	DrawMessages();
	DrawHUD();
}
