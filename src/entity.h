#ifndef _ENTITY_
#define _ENTITY_

/*
Functions and stuff for handling entities
*/

#include "graphics.h"

#define MAXENTITIES		255
#define MAXSTATES		20

enum STATES {ST_IDLE,ST_WALK,ST_JUMP1,ST_JUMP2,ST_DASH,ST_DEAD,ST_DYING,ST_SPEAR,ST_ENEMY,ST_TILE};

typedef struct ENTITY_T
{
	Sprite *sprite;				/*The sprite to be drawn*/
	struct ENTITY_T *owner;		/*For stuff spawned by entities*/
	void (*think) (struct ENTITY_T *self);	/*Called by engine to handle inputs*/
	int used;					/*Is this entity free*/
	int shown;					/*Should this be rendered*/
	int frame;					/*Current frame to render*/
	int state;					/*What state is this in*/
	float sx,sy;				/*Position*/
	float vx,vy;				/*Velocity*/
	SDL_Rect bbox;				/*Bounding box for collisions*/
	
	int uCheck, dCheck, lCheck, rCheck;		/*Collision checks*/
	int isRight;				/*Which way is this facing*/
	int busy;					/*Attack delay*/
	int delay;					/*Animation delay*/
	
	int health;					/*Current health*/
	int healthmax;				/*Max health*/
	int invuln;					/*Delay until you can be hit again*/
}Entity;

/*General Entity Stuff*/
void InitEntityList();
void ClearEntities();
void ClearEntitiesExcept(Entity *skip);
Entity *NewEntity();
void FreeEntity(Entity *ent);
void DrawEntity(Entity *ent);
void DrawEntities();
void UpdateEntities();

/*Specific Entity Stuff*/
Entity *MakePlayer(int x, int y);					/*Player*/
void PlayerThink(Entity *self);
Entity *MakeSpear();								/*Spear*/
void SpearThink(Entity *self);
Entity *SpawnSnake(int x, int y, int i);			/*Snake enemy*/
void SnakeThink(Entity *self);
Entity *SpawnEye(int x, int y, int wave);			/*Eye enemy*/
void EyeThink(Entity *self);
Entity *SpawnCloud();								/*Cloud enemy*/
void CloudThink(Entity *self);
Entity *ShootBolt(int x, int y);					/*Cloud's lightning attack*/
void BoltThink(Entity *self);
Entity *SpawnPixie(int x);							/*Pixie enemy*/
void PixieThink(Entity *self);
Entity *SpawnFrog(int x, int y, int i);				/*Frog enemy*/
void FrogThink(Entity *self);
Entity *SpawnDrill(int x, int y);					/*Drill enemy*/
void DrillThink(Entity *self);
Entity *SpawnBall(int x, int y);					/*Ball enemy*/
void BallThink(Entity *self);
Entity *ShootLaser(int x, int y);					/*Ball's laser attack*/
void LaserThink(Entity *self);
Entity *BuildSnakePot(int x, int y, int i, int j);	/*Snake spawn point*/
void PotThink(Entity *self);
Entity *BuildTile(int x);							/*Ground*/
Entity *BuildSmallTile(int x);						/*Also ground*/
Entity *BuildPlatform(int x, int y);				/*Platforms*/
Entity *BuildWall(int x, int y);					/*Walls*/
Entity *BuildColumn(int x, int y);					/*Columns*/
Entity *BuildCloudPlatform(int x, int y);			/*Cloud platforms*/
void TileThink(Entity *self);
Entity *BuildMovingPlatform(int x, int y, int a, int b);	/*Moving platforms*/
void PlatThink(Entity *self);

/*Keyboard Input Stuff*/
void InitKeyboard();
void ClearKeyboard();
void UpdateKeyboard();
int isKeyPressed(int key);
int isKeyReleased(int key);
int isKeyHeld(int key);

#endif