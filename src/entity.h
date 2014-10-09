#ifndef _ENTITY_
#define _ENTITY_

#include "graphics.h"

#define MAXENTITIES		511
#define MAXSTATES		20
#define SOUNDSPERENT	4

enum STATES {ST_IDLE,ST_WALK,ST_JUMP,ST_DEAD,ST_ENEMY,ST_TILE};

typedef struct ENTITY_T
{
	SPRITE *sprite;
	struct ENTITY_T *owner;
	void (*think) (struct ENTITY_T *self);
	int shown;
	int used;
	int frame;
	int count;
	int state;
	int sx,sy;
	int animframe;
	int busy;
	int delay;
	int vx,vy;
	int framestates[MAXSTATES];
	SDL_Rect bbox;
	int health;
	int healthmax;
}Entity;


void InitEntityList();
Entity *NewEntity();
void FreeEntity(Entity *ent);
void ClearEntities();
void DrawEntities();
void DrawEntity(Entity *ent);
void UpdateEntities();
void UpdateDisplayBar(Entity *player,SPRITE *window);

Entity *MakePlayer();
void PlayerThink(Entity *self);
Entity *MakeTile(int x, int y, int w, int h);
Entity *MakeColumn(int x, int y, int w, int h);


#endif