#ifndef _ENTITY_
#define _ENTITY_

#include "graphics.h"

#define MAXENTITIES		511
#define MAXSTATES		20
#define SOUNDSPERENT	4

enum STATES {ST_IDLE,ST_WALK,ST_JUMP1,ST_JUMP2,ST_DEAD,ST_DYING,ST_SPEAR,ST_ENEMY,ST_TILE};

typedef struct ENTITY_T
{
	Sprite *sprite;
	struct ENTITY_T *owner;
	void (*think) (struct ENTITY_T *self);
	int shown;
	int used;
	int frame;
	int count;
	int state;
	int sx,sy;			/*Position*/
	int animframe;
	int isRight;
	int busy;
	int delay;
	float vx,vy;		/*Velocity*/
	int framestates[MAXSTATES];
	SDL_Rect bbox;
	int health;
	int healthmax;
	int invuln;
}Entity;

/*General Entity Stuff*/
void InitEntityList();
Entity *NewEntity();
void FreeEntity(Entity *ent);
void ClearEntities();
void DrawEntities();
void DrawEntity(Entity *ent);
void UpdateEntities();
void UpdateDisplayBar(Entity *player,Sprite *window);

/*Specific Entity Stuff*/
Entity *MakePlayer();
void PlayerThink(Entity *self);
Entity *MakeSpear(SDL_Rect box, int i);
void SpearThink(Entity *self);
Entity *SpawnSnake(int x, int y);
void SnakeThink(Entity *self);
Entity *BuildTile(int x, int y, int w, int h);
Entity *BuildColumn(int x, int y, int w, int h, int size);

/*Keyboard Inputs*/
void InitKeyboard();
void ClearKeyboard();
void UpdateKeyboard();
int isKeyPressed(int key);
int isKeyReleased(int key);
int isKeyHeld(int key);

#endif