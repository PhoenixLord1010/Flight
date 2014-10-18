#include <stdlib.h>
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
Entity EntityList[MAXENTITIES];
Entity *Player;
int gravity = 13;
int NumLives = 3;
int NumEnts;
int MOUSEMOVE = 1;
int lastx, lasty;
int uCheck2 = 0;

int KeyCount;
Uint8 *oldkeys; /*last frame's key presses*/
Uint8 *keys;    /*current frame's key presses*/


void DrawEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used)
		{
			if(EntityList[i].shown)
				DrawEntity(&EntityList[i]);
		}
	}
}

void UpdateEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used)
		{
			if(EntityList[i].think != NULL)
			{
				EntityList[i].think(&EntityList[i]);
			}
		}
	}
}

void DrawEntity(Entity *ent)
{
	DrawSprite(ent->sprite,screen,ent->sx,ent->sy,ent->frame);
}

void InitEntityList()
{
	int i;
	NumEnts = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		EntityList[i].sprite = NULL;
		EntityList[i].owner = NULL;
		EntityList[i].think = NULL;
		EntityList[i].shown = 0;
		EntityList[i].used = 0;
	}
}

Entity *NewEntity()
{
	int i;
	if(NumEnts + 1 >= MAXENTITIES)
		return NULL;
	NumEnts++;
	for(i = 0; i <= NumEnts; i++)
	{
		if(!EntityList[i].used)break;
	}
	EntityList[i].used = 1;
	return &EntityList[i];
}

void FreeEntity(Entity *ent)
{
	ent->used = 0;
	NumEnts--;
	if(ent->sprite != NULL)FreeSprite(ent->sprite);
	ent->sprite = NULL;
	ent->owner = NULL;
}

void ClearEntities()
{
	int i = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		FreeEntity(&EntityList[i]);
	}
}


int Collide(SDL_Rect box1, SDL_Rect box2)
{
	if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x + box2.w) && (box1.y + box1.h >= box2.y) && (box1.y <= box2.y + box2.h))
		return 1;
	return 0;
}

void CheckCollisions(Entity *self, SDL_Rect box1, int *up, int *down, int *left, int *right, SDL_Rect *xCollision, SDL_Rect *yCollision)
{
	int i = 0;
	SDL_Rect box2;
	*up = 0;
	*down = 0;
	*left = 0;
	*right = 0;

	for(i = 0; i < MAXENTITIES; i++)
	{
		box2.x = EntityList[i].sx + EntityList[i].bbox.x;
		box2.y = EntityList[i].sy + EntityList[i].bbox.y;
		box2.w = EntityList[i].bbox.w;
		box2.h = EntityList[i].bbox.h;

		if(Collide(box1,box2))
		{
			if(EntityList[i].state == ST_TILE)		/*Collision with the world*/
			{
				if((abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)))
				{
					*up = 1;
					*yCollision = box2;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)))
					 {	 
						 *left = 1;
						 *xCollision = box2;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)))
						  {	  
							  *right = 1;
							  *xCollision = box2;
						  }
						  else 
						  {
							  *down = 1;
							  *yCollision = box2;
						  }
			}

			if(self->state == ST_ENEMY && EntityList[i].state == ST_SPEAR)self->health--;	/*Enemy was hit*/
		}
	}
}


Entity *MakePlayer()
{
	Entity *player;
	player = NewEntity();
	if(player == NULL)return player;
	player->sprite = LoadSprite("images/player.png",34,48);
	player->bbox.x = 6;
	player->bbox.y = 0;
	player->bbox.w = 22;
	player->bbox.h = 48;
	player->frame = 0;
	player->sx = 100;
	player->sy = screen->h/1.5;
	player->shown = 1;
	player->delay = 1;
	player->state = ST_IDLE;
	player->isRight = 1;
	player->think = PlayerThink;
	player->health = 10;
	player->healthmax = 10;
	player->invuln = 0;
	player->owner = player;
	Player = player;
	return player;
}

void PlayerThink(Entity *self)
{
	/*Collision Stuff*/
	SDL_Rect b1, xCol, yCol;
	int uCheck, dCheck, lCheck, rCheck;
	
	/*Spear Stuff*/
	int sFrame;
	SDL_Rect sBox;

	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y + self->vy/1.5;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	CheckCollisions(self, b1, &uCheck, &dCheck, &lCheck, &rCheck, &xCol, &yCol);

	if(!self->isRight && self->state != ST_DYING)self->frame -= 8;	/*Direction Calculation*/


	/*Do While Alive*/
	if(self->health > 0)
	{
		if(self->vx != 0)	/*Decelerates X Movement When Nothing's Pressed*/
		{
			if(self->vx > 0)
			{
				if(self->vx >= 3)
				{
					if(self->invuln == 0)
						self->vx -= 2.5;
					else self->vx -= 1;
				}
				else self->vx = 0;
			}
			else
			{
				if(self->vx <= -3)
				{
					if(self->invuln == 0)
						self->vx += 2.5;
					else self->vx += 1;
				}
				else self->vx = 0;
			}
		}

		if(self->vy <= gravity && !uCheck)	/*Gravity*/
			self->vy += 2;

		/*What to Do if Colliding*/
		if(uCheck)
		{
			self->vy = 0;
			self->sy = yCol.y - self->bbox.h;
		}
		if(dCheck)
		{
			self->vy = 1;
			self->sy = yCol.y + yCol.h;
		}
		if(lCheck)
		{
			self->sx = xCol.x - 32;
		}
		if(rCheck)
		{
			self->sx = xCol.x + xCol.w;
		}
		

		self->sx += self->vx;
		self->sy += self->vy;

		if((self->vx == 0) && (self->vy == 0) && uCheck)
			self->state = ST_IDLE;

		if(uCheck2 && !uCheck)
		{
			self->state = ST_JUMP1;
			self->frame = 4;
		}

		/*Player Inputs*/
		if(self->invuln <= 15)
		{
			if(isKeyHeld(SDLK_a) && !rCheck && self->sx > 0)	/*Move left*/
			{
				self->isRight = 0;
				if(self->vx > -10)self->vx -= 5;
				if(uCheck)self->state = ST_WALK;
			}
			if(isKeyHeld(SDLK_d) && !lCheck)					/*Move right*/
			{
				self->isRight = 1;
				if(self->vx < 10)self->vx += 5;
				if(uCheck)self->state = ST_WALK;
			}

			if(isKeyPressed(SDLK_KP0) && self->vy >= 0 && self->state != ST_JUMP2)	/*Jumps*/
			{
				if(self->state == ST_JUMP2)self->vy = -30;
				else self->vy = -20;
				if(self->state == ST_JUMP1)self->state = ST_JUMP2;
				else
				{
					self->state = ST_JUMP1;
					self->frame = 4;
				}
				uCheck = 0;
			}
		
			if(isKeyHeld(SDLK_KP0) && self->vy > 0)gravity = 1.5;		/*Float*/
			else gravity = 13;


			/*Directional Attack*/
			if(isKeyPressed(SDLK_KP6) || isKeyPressed(SDLK_KP5))	/*Right*/
			{
				sBox.x = 68;
				sBox.y = 38;
				sFrame = 0;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP9))	/*Up-Right*/
			{
				sBox.x = 60;
				sBox.y = 12;
				sFrame = 1;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP3))	/*Down-Right*/
			{
				sBox.x = 60;
				sBox.y = 64;
				sFrame = 2;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP4))	/*Left*/
			{
				sBox.x = 0;
				sBox.y = 38;
				sFrame = 3;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP7))	/*Up-Left*/
			{
				sBox.x = 8;
				sBox.y = 12;
				sFrame = 4;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP1))	/*Down-Left*/
			{
				sBox.x = 8;
				sBox.y = 64;
				sFrame = 5;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP8))	/*Up*/
			{
				sBox.x = 34;
				sBox.y = 0;
				sFrame = 6;
				MakeSpear(sBox, sFrame);
			}
			if(isKeyPressed(SDLK_KP2))	/*Down*/
			{
				sBox.x = 34;
				sBox.y = 76;
				sFrame = 7;
				MakeSpear(sBox, sFrame);
			}				
		}

		uCheck2 = uCheck;
	}

	if((self->health <= 0) && (self->state != ST_DYING))		/*You died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = 0;
		self->state = ST_DYING;
		self->frame = 16;
		self->delay = 15;
	}
	
	if(self->sy >= 800)self->state = ST_DEAD;	/*How you actually die*/
	

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			exit(1);
			break;
		case ST_DYING:
			if(self->delay <= 0)
			{
				if(self->delay == 0)
				{
					self->vy = -20;
					self->delay--;
				}
				else
				{
					self->vy += 2;
					self->sy += self->vy;
				}
			}
			else self->delay--;
			break;
		case ST_IDLE:
			self->frame = 0;
			break;
		case ST_WALK:
			if(self->frame > 3)self->frame = 1;
			if((self->frame <= 3) && (self->delay <= 0))
			{
				self->frame++;
				self->delay = 1;
				if(self->frame > 3)self->frame = 0; 
			}
			else self->delay--;
			break;
		case ST_JUMP1:
			if(self->delay <= 0)
			{
				self->delay = 7;
				switch(self->frame)
				{
					case 4: self->frame = 5;
						break;
					case 5: self->frame = 4;
						break;
				}
			}
			else self->delay--;
			break;
		case ST_JUMP2:
			if(self->delay <= 0)
			{
				self->delay = 3;
				switch(self->frame)
				{
					case 4: self->frame = 5;
						break;
					case 5: self->frame = 4;
						break;
				}
			}
			else self->delay--;
			break;
	}

	if(!self->isRight && self->state != ST_DYING)self->frame += 8;		/*Face left*/
	if(self->invuln > 0)self->invuln--;		/*Invulnerability counter*/
}


Entity *MakeSpear(SDL_Rect box, int i)
{
	Entity *spear;
	spear = NewEntity();
	if(spear == NULL)return spear;
	spear->sprite = LoadSprite("images/spear.png",80,88);
	spear->bbox.x = box.x;
	spear->bbox.y = box.y;
	spear->bbox.w = 12;
	spear->bbox.h = 12;
	spear->frame = i;
	spear->sx = Player->sx - 23;
	spear->sy = Player->sy - 20;
	spear->shown = 1;
	spear->delay = 10;
	spear->state = ST_SPEAR;
	spear->think = SpearThink;
	return spear;
}

void SpearThink(Entity *self)
{
	self->sx = Player->sx - 23;
	self->sy = Player->sy - 20;

	if(self->delay >= 0)self->delay--;
	else FreeEntity(self);
}


Entity *SpawnSnake(int x, int y)
{
	Entity *enemy;
	enemy = NewEntity();
	if(enemy == NULL)return enemy;
	enemy->sprite = LoadSprite("images/snake.png",20,32);
	enemy->bbox.x = 2;
	enemy->bbox.y = 0;
	enemy->bbox.w = 16;
	enemy->bbox.h = 32;
	enemy->frame = 0;
	enemy->sx = x;
	enemy->sy = y;
	enemy->vx = -3;
	enemy->vy = 0;
	enemy->shown = 1;
	enemy->delay = 1;
	enemy->health = 1;
	enemy->healthmax = 1;
	enemy->state = ST_ENEMY;
	enemy->isRight = 0;
	enemy->think = SnakeThink;
	enemy->owner = NULL;
	return enemy;
}

void SnakeThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	int uCheck, dCheck, lCheck, rCheck;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y + self->vy/1.5;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &uCheck, &dCheck, &lCheck, &rCheck, &xCol, &yCol);

	if(self->isRight)self->frame -= 3;

	if(self->health > 0)
	{
		if(self->vy <= gravity && !uCheck)	/*Gravity*/
			self->vy += 2;

		/*What to Do if Colliding*/
		if(uCheck)
		{
			self->vy = 0;
			self->sy = yCol.y - self->bbox.h;
		}
		if(lCheck)
		{
			self->vx = -1 * abs(self->vx);
		}
		if(rCheck)
		{
			self->vx = abs(self->vx);
		}
		
		if(self->vx < 0)		/*Move left*/
			self->isRight = 0;
		if(self->vx >0)			/*Move right*/
			self->isRight = 1;

		self->sx += self->vx;
		self->sy += self->vy;

		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)	/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
				Player->vx = -15;
			else Player->vx = 15;
			Player->invuln = 30;
		}
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2;
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			if(self->sy >= 800)FreeEntity(self);
			break;
		case ST_ENEMY:
			if(self->delay <= 0)
			{
				self->delay = 3;
				switch(self->frame)
				{
					case 0: self->frame = 1;
						break;
					case 1: self->frame = 0;
						break;
				}
			}
			else self->delay--;
			break;
	}

	if(self->isRight)self->frame += 3;		/*Face right*/
}


Entity *BuildTile(int x, int y, int w, int h)
{
	Entity *tile;
	tile = NewEntity();
	if(tile == NULL)return tile;
	tile->sprite = LoadSprite("images/tile.png", w, h);
	tile->shown = 1;
	tile->sx = x;
	tile->sy = y;
	tile->bbox.x = 0;
	tile->bbox.y = 0;
	tile->bbox.w = w;
	tile->bbox.h = h;
	tile->state = ST_TILE;
	return tile;
}

Entity *BuildColumn(int x, int y, int w, int h, int size)
{
	Entity *column;
	column = NewEntity();
	if(column == NULL)return column;
	if(size == 1)column->sprite = LoadSprite("images/small_column.png", w, h);
	if(size == 2)column->sprite = LoadSprite("images/big_column.png", w, h);
	column->shown = 1;
	column->sx = x;
	column->sy = y;
	column->bbox.x = 0;
	column->bbox.y = 0;
	column->bbox.w = w;
	column->bbox.h = h;
	column->state = ST_TILE;
	return column;
}


void InitKeyboard()
{
  keys = SDL_GetKeyState(&KeyCount);
  oldkeys = (Uint8 *)malloc(sizeof(Uint8)*KeyCount);
  if(oldkeys == NULL)
  {
    fprintf(stderr,"unable to initialize keyboard structure!\n");
  }
}

void ClearKeyboard()
{
  if(oldkeys == NULL)return;
  memset(oldkeys,0,sizeof(Uint8)*KeyCount);
}

void UpdateKeyboard()
{
  int i;
  if((oldkeys == NULL)||(keys == NULL))
  {
    return;
  }
  for(i = 0; i < KeyCount;i++)
  {
    oldkeys[i] = keys[i];
  }
  keys = SDL_GetKeyState(NULL);
}

int isKeyPressed(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((keys[key]) && (!oldkeys[key]))
  {
    return 1;
  }
  return 0;
}

int isKeyReleased(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((!keys[key]) && (oldkeys[key]))
  {
    return 1;
  }
  return 0;
}

int isKeyHeld(int key)
{
  if((oldkeys == NULL)||(keys == NULL))
  {
    return 0;
  }
  if((keys[key]) && (oldkeys[key]))
  {
    return 1;
  }
  return 0;
}
