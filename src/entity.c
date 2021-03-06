#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
extern SDL_Rect Camera;
extern float offset;
extern float onset;
extern float quake;

Entity EntityList[MAXENTITIES];
Entity *Player;
Entity *Boss;
int gravity = 13;
int NumLives = 3;
int NumEnts;
int MOUSEMOVE = 1;
int lastx, lasty;
int uCheck2 = 0;
float empty;

int KeyCount;
Uint8 *oldkeys; /*last frame's key presses*/
Uint8 *keys;    /*current frame's key presses*/


void InitEntityList()
{
	int i, j;
	NumEnts = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		EntityList[i].used = 0;
		EntityList[i].layer = 10;
		EntityList[i].think = NULL;
		if(EntityList[i].sprite != NULL)FreeSprite(EntityList[i].sprite);
		for(j = 0;j < SOUNDSPERENT;j++)
		{
			EntityList[i].sound[j] = NULL;
		}
		EntityList[i].sprite = NULL;
		EntityList[i].owner = NULL;
	}
	memset(EntityList, 0, sizeof(Entity) * MAXENTITIES);
}

void ClearEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used)
			FreeEntity(&EntityList[i]);
	}
}

void ClearEntitiesExcept(Entity *skip)
{
  int i;
  for(i = 0; i < MAXENTITIES; i++)
  {
    if((EntityList[i].used) && (skip != &EntityList[i]))
    {
      FreeEntity(&EntityList[i]);
    }
  }
}

Entity *NewEntity()
{
	int i;
	if((NumEnts + 1) >= MAXENTITIES)
		return NULL;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(!EntityList[i].used)
		{
			memset(&EntityList[i], 0, sizeof(Entity));
			EntityList[i].used = 1;
			EntityList[i].layer = 1;
			EntityList[i].shown = 1;
			EntityList[i].invuln = 0;
			NumEnts++;
			return &EntityList[i];
		}
	}
	return NULL;
}

void FreeEntity(Entity *ent)
{
	int j;
	NumEnts--;
	ent->used = 0;
	ent->layer = 10;
	if(ent->sprite != NULL)FreeSprite(ent->sprite);
	for(j = 0;j < SOUNDSPERENT;j++)
	{
		if(ent->sound[j] != NULL)FreeSound(ent->sound[j]);
		ent->sound[j] = NULL;
	}
	memset(ent, 0, sizeof(Entity));
}

void DrawEntity(Entity *ent)
{
	DrawSprite(ent->sprite, screen, ent->sx, ent->sy, ent->frame);
}


void DrawEntities()
{
	int i,j;
	for(j = 0; j < 10; j++)
	{
		for(i = 0; i < MAXENTITIES; i++)
		{
			if((EntityList[i].used > 0) && (EntityList[i].shown > 0) && (EntityList[i].layer == j))
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


int Collide(SDL_Rect box1, SDL_Rect box2)
{
	if((box1.x + box1.w >= box2.x) && (box1.x <= box2.x + box2.w) && (box1.y + box1.h >= box2.y) && (box1.y <= box2.y + box2.h))
		return 1;
	return 0;
}

void CheckCollisions(Entity *self, SDL_Rect box1, SDL_Rect *xCollision, SDL_Rect *yCollision, float *yMovement)
{
	int i = 0;
	SDL_Rect box2;
	
	self->uCheck = 0;
	self->dCheck = 0;
	self->lCheck = 0;
	self->rCheck = 0;

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
				if((abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)) && (self->vy - EntityList[i].vy) >= 0)
				{
					self->uCheck = 1;
					*yCollision = box2;
					*yMovement = EntityList[i].vy;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)) && ((self->vx - EntityList[i].vx) >= 0) && !EntityList[i].invuln)
					 {	 
						 self->lCheck = 1;
						 *xCollision = box2;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)) && ((self->vx - EntityList[i].vx) <= 0) && !EntityList[i].invuln)
						  {	  
							  self->rCheck = 1;
							  *xCollision = box2;
						  }
						  else if((self->vy - EntityList[i].vy) <= 0 && !EntityList[i].invuln)
							   {
								   self->dCheck = 1;
								   *yCollision = box2;
							   }
			}

			if(self->state == ST_ENEMY && EntityList[i].state == ST_SPEAR)self->invuln = 1;	/*Enemy was hit*/
		}
	}
}


Entity *MakePlayer(int x, int y)
{
	Entity *player;
	player = NewEntity();
	if(player == NULL)return player;
	player->sprite = LoadSprite("images/player.png",34,48);
	player->sound[0] = LoadSound("sounds/player_death.wav", MIX_MAX_VOLUME/2);
	player->sound[1] = LoadSound("sounds/player_jump.wav", MIX_MAX_VOLUME/2);
	player->bbox.x = 6;
	player->bbox.y = 0;
	player->bbox.w = 22;
	player->bbox.h = 48;
	player->frame = 0;
	player->sx = x;
	player->sy = y;
	player->shown = 1;
	player->delay = 1;
	player->busy = 0;
	player->ct = 0;
	player->state = ST_IDLE;
	player->isRight = 1;
	player->think = PlayerThink;
	player->health = 5;
	player->healthmax = 5;
	player->owner = player;
	Player = player;
	MakeSpear();
	return player;
}

void PlayerThink(Entity *self)
{
	/*Collision Stuff*/
	int i = 0;
	SDL_Rect b1, xCol, yCol;
	float yMov;
	
	/*Spear Stuff*/
	int sFrame;
	SDL_Rect sBox;

	
	/*Check for Collisions*/
	do
	{
		b1.x = self->sx + self->bbox.x + (self->vx * (i / 10));
		b1.y = self->sy + self->bbox.y + self->vy/1.5;
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		CheckCollisions(self, b1, &xCol, &yCol, &yMov);
		i++;
	}
	while((i <= 10) /*&& (!self->lCheck && !self->rCheck)*/);

	if(!self->isRight && self->state != ST_DYING)self->frame -= 8;	/*Direction Calculation*/


	/*Do While Alive*/
	if(self->health > 0)
	{
		if(self->vx != 0)	/*Decelerates X Movement When Nothing's Pressed*/
		{
			if(self->vx > 0)
			{
				if(self->vx >= 2.5)
				{
					if(self->invuln == 0)
						self->vx -= 2.5;
					else self->vx -= 1;
				}
				else self->vx = 0;
			}
			else
			{
				if(self->vx <= -2.5)
				{
					if(self->invuln == 0)
						self->vx += 2.5;
					else self->vx += 1;
				}
				else self->vx = 0;
			}
		}

		if(self->vy <= gravity && !self->uCheck)	/*Gravity*/
			self->vy += 2;

		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			self->vy = yMov;
			self->sy = yCol.y - self->bbox.h;
		}
		if(self->dCheck)
		{
			self->vy = 2;
			self->sy = yCol.y + yCol.h;
		}
		if(self->lCheck)
		{
			self->vx = -1;
			self->sx = xCol.x - 32;
		}
		if(self->rCheck)
		{
			self->vx = 1;
			self->sx = xCol.x + xCol.w;
		}
		
		/*Keep player on screen*/
		if(self->sx < offset)self->sx = offset;
		if(self->sx + self->sprite->w > screen->w + offset)self->sx = screen->w + offset - self->sprite->w;
		if(self->sy <= 0)self->vy = 1;

		self->sx += self->vx;
		self->sy += self->vy;

		if((self->vx == 0) && self->uCheck)	/*Standing Still*/
			self->state = ST_IDLE;
		
		if(uCheck2 && !self->uCheck)	/*Walking off Edges*/
		{
			if(self->state != ST_DASH)
			{
				self->state = ST_JUMP1;
				self->frame = 4;
			}
		}

		/*Player Inputs*/
		if(self->invuln <= 15)
		{
			if(isKeyHeld(SDLK_e) && !self->rCheck)					/*Move left*/
			{
				if(self->busy <= 7)self->isRight = 0;
				if(self->vx > -8)self->vx -= 4;
				if(self->uCheck && self->state != ST_DASH)self->state = ST_WALK;
			}
			if(isKeyHeld(SDLK_t) && !self->lCheck)					/*Move right*/
			{
				if(self->busy <= 7)self->isRight = 1;
				if(self->vx < 8)self->vx += 4;
				if(self->uCheck && self->state != ST_DASH)self->state = ST_WALK;
			}

			if(isKeyPressed(SDLK_KP0) && self->vy >= 0 && self->state != ST_JUMP2)			/*Jumps*/
			{
				if(self->state == ST_JUMP2)self->vy = -30;
				else self->vy = -20;
				if(self->state == ST_JUMP1)self->state = ST_JUMP2;
				else
				{
					self->state = ST_JUMP1;
					self->frame = 4;
				}
				Mix_PlayChannel(-1,self->sound[1]->sound,0);
				self->uCheck = 0;
			}
		
			if(isKeyHeld(SDLK_KP0) && self->vy > 0)		/*Float*/
			{
				gravity = 1.5 + (0.05 * self->ct);
				self->ct++;
			}
			else
			{
				gravity = 13;
				self->ct = 0;
			}
		}

		if(isKeyPressed(SDLK_KP6) && self->busy <= 0)	/*Right*/
		{
			self->isRight = 1;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP9) && self->busy <= 0)	/*Up-Right*/
		{
			self->isRight = 1;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP3) && self->busy <= 0)	/*Down-Right*/
		{
			self->isRight = 1;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP4) && self->busy <= 0)	/*Left*/
		{
			self->isRight = 0;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP7) && self->busy <= 0)	/*Up-Left*/
		{
			self->isRight = 0;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP1) && self->busy <= 0)	/*Down-Left*/
		{
			self->isRight = 0;
			self->busy = 15;
		}
		if(isKeyPressed(SDLK_KP8) && self->busy <= 0)self->busy = 15;	/*Up*/
		if(isKeyPressed(SDLK_KP2) && self->busy <= 0)self->busy = 15;	/*Down*/

		if((isKeyPressed(SDLK_KP_PLUS) || isKeyPressed(SDLK_KP_ENTER)) && self->busy <= 0 && self->uCheck && !self->lCheck && !self->rCheck)	/*Dash*/
		{
			if(self->isRight)
				self->vx = 30;
			else 
				self->vx = -30;
			self->state = ST_DASH;
			self->frame = 6;
			self->busy = 15;
		}

		if(self->busy >= 0)self->busy--;
		uCheck2 = self->uCheck;
	}

	if((self->health <= 0) && (self->state != ST_DYING))		/*You died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = 0;
		self->state = ST_DYING;
		self->frame = 16;
		self->delay = 15;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}
	
	if(self->sy >= 800)		/*How you actually die*/
		self->state = ST_DEAD;
	

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->health = 0;
			return;
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
		case ST_DASH:
			if(self->delay <= 0)
			{
				self->delay = 2;
				switch(self->frame)
				{
					case 6: self->frame = 7;
						break;
					case 7: self->frame = 6;
						break;
				}
			}
			else self->delay--;
			self->uCheck = 1;
			if(self->busy == 1 && uCheck2)self->state = ST_IDLE;
			break;
	}

	if(!self->isRight && self->state != ST_DYING)self->frame += 8;		/*Face left*/
	if(self->invuln > 0)self->invuln--;		/*Invulnerability counter*/
}


Entity *MakeSpear()
{
	Entity *spear;
	spear = NewEntity();
	if(spear == NULL)return spear;
	spear->sprite = LoadSprite("images/spear.png",80,88);
	spear->sound[0] = LoadSound("sounds/spear.wav", MIX_MAX_VOLUME/2);
	spear->bbox.x = 68;
	spear->bbox.y = 38;
	spear->bbox.w = 12;
	spear->bbox.h = 12;
	spear->frame = 0;
	spear->sx = Player->sx - 23;
	spear->sy = Player->sy - 20;
	spear->shown = 0;
	spear->busy = 0;
	spear->state = ST_SPEAR;
	spear->think = SpearThink;
	spear->owner = Player;
	return spear;
}

void SpearThink(Entity *self)
{
	if(Player == NULL)FreeEntity(self);
	
	/*Always move with the player*/
	self->sx = Player->sx - 23;
	self->sy = Player->sy - 20;

	/*Directional Attack*/
	if(Player->health > 0)
	{
		if(isKeyPressed(SDLK_KP6) && self->busy <= 0)	/*Right*/
		{
			self->bbox.x = 68;
			self->bbox.y = 38;
			self->frame = 0;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 1;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP9) && self->busy <= 0)	/*Up-Right*/
		{
			self->bbox.x = 60;
			self->bbox.y = 12;
			self->frame = 1;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 1;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP3) && self->busy <= 0)	/*Down-Right*/
		{
			self->bbox.x = 60;
			self->bbox.y = 64;
			self->frame = 2;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 1;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP4) && self->busy <= 0)	/*Left*/
		{
			self->bbox.x = 0;
			self->bbox.y = 38;
			self->frame = 3;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 0;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP7) && self->busy <= 0)	/*Up-Left*/
		{
			self->bbox.x = 8;
			self->bbox.y = 12;
			self->frame = 4;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 0;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP1) && self->busy <= 0)	/*Down-Left*/
		{
			self->bbox.x = 8;
			self->bbox.y = 64;
			self->frame = 5;
			self->shown = 1;
			self->state = ST_SPEAR;
			Player->isRight = 0;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP8) && self->busy <= 0)	/*Up*/
		{
			self->bbox.x = 34;
			self->bbox.y = 0;
			self->frame = 6;
			self->shown = 1;
			self->state = ST_SPEAR;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
		if(isKeyPressed(SDLK_KP2) && self->busy <= 0)	/*Down*/
		{
			self->bbox.x = 34;
			self->bbox.y = 76;
			self->frame = 7;
			self->shown = 1;
			self->state = ST_SPEAR;
			self->busy = 15;
			Mix_PlayChannel(-1,self->sound[0]->sound,0);
		}
			
		if((isKeyPressed(SDLK_KP_PLUS) || isKeyPressed(SDLK_KP_ENTER)) && self->busy <= 0 && Player->uCheck == 1)	/*Dash*/
		{
			if(Player->isRight)
			{
				self->bbox.x = 68;
				self->bbox.y = 38;
				self->frame = 0;
			}
			else 
			{
				self->bbox.x = 0;
				self->bbox.y = 38;
				self->frame = 3;
			}
			self->shown = 1;
			self->state = ST_SPEAR;
			self->busy = 15;
		}

		/*Cooldown*/
		if(self->busy <= 8)
		{
			self->shown = 0;
			self->state = ST_IDLE;
		}
		if(self->busy >= 0)self->busy--;
	}
}


Entity *SpawnSnake(int x, int y, int i)
{
	Entity *snake;
	snake = NewEntity();
	if(snake == NULL)return snake;
	snake->sprite = LoadSprite("images/snake.png",20,32);
	snake->sound[0] = LoadSound("sounds/snake_death.wav", MIX_MAX_VOLUME/2);
	snake->bbox.x = 2;
	snake->bbox.y = 0;
	snake->bbox.w = 16;
	snake->bbox.h = 32;
	snake->frame = 0 + (3 * i);
	snake->sx = x;
	snake->sy = y;
	if(i==0)snake->vx = -3;
	else snake->vx = 3;
	snake->vy = 0;
	snake->shown = 1;
	snake->delay = 1;
	snake->health = 1;
	snake->healthmax = 1;
	snake->state = ST_ENEMY;
	snake->isRight = i;
	snake->think = SnakeThink;
	snake->owner = NULL;
	return snake;
}

void SnakeThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(self->health > 0)
	{
		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			self->vy = 0;
			self->sy = yCol.y - self->bbox.h;
		}
		if(self->lCheck)
		{
			self->isRight = 0;
			self->frame -= 3;
		}
		if(self->rCheck)
		{
			self->isRight = 1;
			self->frame += 3;
		}
		
		if(self->isRight)		/*Move right*/
			self->vx = 3;
		else					/*Move left*/
			self->vx = -3;

		if(self->vy <= 13 && !self->uCheck)	/*Gravity*/
		{
			if(self->isRight)self->vx = 2;
			else self->vx = -2;
			self->vy += 2;
		}

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

		if(self->invuln)self->health--;
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2 + (3 * self->isRight);
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			break;
		case ST_ENEMY:
			if(self->delay <= 0)
			{
				self->delay = 3;
				switch(self->frame - (3 * self->isRight))
				{
					case 0: self->frame = 1 + (3 * self->isRight);
						break;
					case 1: self->frame = 0 + (3 * self->isRight);
						break;
				}
			}
			else self->delay--;
			break;
	}

	if(self->sx + self->bbox.w < onset || self->sy > 800)FreeEntity(self);		/*When offscreen, free self*/
}

Entity *BuildSnakePot(int x, int y, int i, int j)
{
	Entity *pot;
	pot = NewEntity();
	if(pot == NULL)return pot;
	pot->sprite = LoadSprite("images/snakepot.png", 32, 64);
	pot->isRight = i;
	pot->frame = 8 * i;
	pot->shown = 1;
	pot->sx = x;
	pot->sy = y;
	pot->bbox.x = 0;
	pot->bbox.y = 32;
	pot->bbox.w = 32;
	pot->bbox.h = 32;
	pot->delay = 0;
	pot->busy = j;				/*Each pot can have its own spawn rate*/
	pot->state = ST_TILE;
	pot->think = PotThink;
	pot->owner = NULL;
	return pot;
}

void PotThink(Entity *self)
{
	if(self->delay <= 0)
	{
		self->delay = 2;
		if(self->frame < 7 + (8 * self->isRight))
		{
			self->frame++;
		}
		else
		{
			self->frame = 0 + (8 * self->isRight);
			SpawnSnake(self->sx + 6, self->sy, self->isRight);
			self->delay = self->busy;
		}
	}
	else self->delay--;
}

Entity *SpawnEye(int x, int y, int wave)
{
	Entity *eye;
	eye = NewEntity();
	if(eye == NULL)return eye;
	eye->sprite = LoadSprite("images/eye.png",32,32);
	eye->sound[0] = LoadSound("sounds/eye_death.wav", MIX_MAX_VOLUME/2);
	eye->bbox.x = 0;
	eye->bbox.y = 0;
	eye->bbox.w = 32;
	eye->bbox.h = 32;
	eye->frame = 0;
	eye->sx = x;
	eye->sy = y;
	eye->vx = -4;
	eye->vy = 0;
	eye->shown = 1;
	eye->delay = 15 * wave;		/*In order to have the same wave, a group starts at the same place with slight delays for each member*/
	eye->health = 1;
	eye->healthmax = 1;
	eye->state = ST_ENEMY;
	eye->isRight = 0;
	eye->think = EyeThink;
	eye->owner = NULL;
	return eye;
}

void EyeThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(self->health > 0 && self->delay <= 0)
	{	
		self->vy = 2 * sin(float(self->sx / 100));	/*Wave motion*/
		
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

		if(self->invuln)self->health--;
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 1;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	if(self->delay > 0)self->delay--;

	if(self->sx + self->bbox.w < onset)FreeEntity(self);	/*Off Screen*/

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			if(self->sy >= 800)FreeEntity(self);
			break;
		case ST_ENEMY:
			self->frame = 0;
			break;
	}
}

Entity *SpawnCloud()
{
	Entity *cloud;
	cloud = NewEntity();
	if(cloud == NULL)return cloud;
	cloud->sprite = LoadSprite("images/cloud.png",32,32);
	cloud->sound[0] = LoadSound("sounds/cloud_death.wav", MIX_MAX_VOLUME/2);
	cloud->sound[1] = LoadSound("sounds/cloud_jolt.wav", MIX_MAX_VOLUME/2);
	cloud->bbox.x = 0;
	cloud->bbox.y = 0;
	cloud->bbox.w = 32;
	cloud->bbox.h = 32;
	cloud->frame = 0;
	cloud->sx = Player->sx;
	cloud->sy = Player->sy - 100;
	cloud->shown = 1;
	cloud->delay = 10;
	cloud->health = 1;
	cloud->healthmax = 1;
	cloud->state = ST_IDLE;
	cloud->think = CloudThink;
	cloud->owner = NULL;
	return cloud;
}

void CloudThink(Entity *self)
{
	if(self->health > 0)
	{
		SDL_Rect b1, b2, xCol, yCol;
	
		/*Check for Collisions*/
		b1.x = self->sx + self->bbox.x;
		b1.y = self->sy + self->bbox.y;
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		b2.x = Player->sx + Player->bbox.x;
		b2.y = Player->sy + Player->bbox.y;
		b2.w = Player->bbox.w;
		b2.h = Player->bbox.h;
		CheckCollisions(self, b1, &xCol, &yCol, &empty);
		
		if(self->state == ST_IDLE)
		{
			self->sx = Player->sx;
			self->sy = Player->sy - 100;
		}

		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0 && self->state == ST_ENEMY)	/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
				Player->vx = -15;
			else Player->vx = 15;
			Player->invuln = 30;
		}

		if(self->invuln)self->health--;
	}
	
	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 6;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			if(self->sy >= 800)FreeEntity(self);
			break;
		case ST_DYING:
			if(self->delay <= 0)
			{
				self->delay = 5;
				if(self->frame > 0)
					self->frame--;
				else
					self->state = ST_IDLE;
			}
			else self->delay--;
			break;
		case ST_ENEMY:
			if(self->delay < 0)
			{
				ShootBolt(self->sx + 8, self->sy + 16);
				Mix_PlayChannel(-1,self->sound[1]->sound,0);
				self->delay = 30;
			}
			else if(self->delay == 0)
					self->state = ST_DYING;
				 else self->delay--;
			break;
		case ST_IDLE:
			if(self->delay <= 0)
			{
				self->delay = 10;
				if(self->frame < 5)
					self->frame++;
				else
				{
					self->state = ST_ENEMY;
					self->delay = -1;
				}
			}
			else self->delay--;
			break;
	}
}

Entity *ShootBolt(int x, int y)
{
	Entity *bolt;
	bolt = NewEntity();
	if(bolt == NULL)return bolt;
	bolt->sprite = LoadSprite("images/bolt.png", 16, 16);
	bolt->bbox.x = 0;
	bolt->bbox.y = 0;
	bolt->bbox.w = 16;
	bolt->bbox.h = 16;
	bolt->frame = 0;
	bolt->sx = x;
	bolt->sy = y;
	bolt->vy = 5;
	bolt->shown = 1;
	bolt->delay = 2;
	bolt->state = ST_ENEMY;
	bolt->think = BoltThink;
	bolt->owner = NULL;
	return bolt;
}

void BoltThink(Entity *self)
{
	SDL_Rect b1, b2;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;

	if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)	/*Check for Player Collision*/
	{
		Player->health -= 1;
		Player->vy = -15;
		if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
			Player->vx = -15;
		else Player->vx = 15;
		Player->invuln = 30;
	}

	self->sy += self->vy;
	if(self->sy > 800)FreeEntity(self);

	if(self->delay <= 0)		/*Animations*/
	{
		self->delay = 1;
		if(self->frame < 4)self->frame++;
		else self->frame = 0;
	}
	else self->delay--;
}

Entity *SpawnPixie(int x)
{
	Entity *pixie;
	pixie = NewEntity();
	if(pixie == NULL)return pixie;
	pixie->sprite = LoadSprite("images/pixie.png",32,32);
	pixie->sound[0] = LoadSound("sounds/pixie_death.wav", MIX_MAX_VOLUME/2);
	pixie->bbox.x = 0;
	pixie->bbox.y = 0;
	pixie->bbox.w = 32;
	pixie->bbox.h = 32;
	pixie->frame = 0;
	pixie->sx = x;
	pixie->sy = -32;
	pixie->vx = -2;
	pixie->vy = 20;
	pixie->shown = 1;
	pixie->delay = 10;
	pixie->busy = 0;
	pixie->health = 1;
	pixie->healthmax = 1;
	pixie->state = ST_ENEMY;
	pixie->think = PixieThink;
	pixie->owner = NULL;
	return pixie;
}

void PixieThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(Player->sx + 480 > self->sx)self->busy = 1;
	
	if(self->health > 0 && self->busy)
	{	
		if(self->sy < 600 && self->sy < Player->sy)
		{
			self->vy -= 0.3;
		}
		else
		{
			self->vx = -15;
			self->vy = 0;
			self->busy = self->sy;
		}
		
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

		if(self->invuln)self->health--;
	}
	
	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
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
				if(self->sy < self->busy)self->delay = 6;
				else self->delay = 2;
				switch(self->frame - (3 * self->isRight))
				{
					case 0: self->frame = 1 + (3 * self->isRight);
						break;
					case 1: self->frame = 0 + (3 * self->isRight);
						break;
				}
			}
			else self->delay--;
			break;
	}
}

Entity *SpawnFrog(int x, int y, int i)
{
	Entity *frog;
	frog = NewEntity();
	if(frog == NULL)return frog;
	frog->sprite = LoadSprite("images/frog.png",28,28);
	frog->sound[0] = LoadSound("sounds/frog_death.wav", MIX_MAX_VOLUME/2);
	frog->sound[1] = LoadSound("sounds/frog_jump.wav", MIX_MAX_VOLUME/2);
	frog->bbox.x = 2;
	frog->bbox.y = 4;
	frog->bbox.w = 24;
	frog->bbox.h = 24;
	frog->frame = 0 + (3 * i);
	frog->sx = x;
	frog->sy = y;
	frog->vx = 2;
	frog->vy = -15;
	frog->shown = 1;
	frog->delay = 10;	/*pauses between jumps for this many frames*/
	frog->busy = 1;
	frog->health = 1;
	frog->healthmax = 1;
	frog->state = ST_ENEMY;
	frog->isRight = i;
	frog->think = FrogThink;
	frog->owner = NULL;
	return frog;
}

void FrogThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(self->health > 0)
	{
		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			if(self->delay == 0)		/*Jump*/
			{
				self->busy = 1;
				self->delay = 10;
				self->vy = -12;
				Mix_PlayChannel(-1,self->sound[1]->sound,0);
			}
			else 
			{
				self->busy = 0;
				self->delay--;
			}
			self->sy = yCol.y - self->sprite->h;
		}
		else self->vy += 1;
		if(self->lCheck)
		{
			self->isRight = 0;
			self->frame -= 3;
		}
		if(self->rCheck)
		{
			self->isRight = 1;
			self->frame += 3;
		}
		
		if(self->isRight)		/*Move right*/
			self->vx =  abs(self->vx);
		else					/*Move left*/
			self->vx = -1 * abs(self->vx);

		self->sx += self->vx * self->busy;
		self->sy += self->vy * self->busy;

		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)	/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
				Player->vx = -15;
			else Player->vx = 15;
			Player->invuln = 30;
		}

		if(self->invuln)self->health--;
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2 + (3 * self->isRight);
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			break;
		case ST_ENEMY:
			if(self->uCheck)self->frame = 0 + (3 * self->isRight);
			else self->frame = 1 + (3 * self->isRight);
			break;
	}

	if(self->sx + self->bbox.w < onset || self->sy > 800)FreeEntity(self);		/*When offscreen, free self*/
}

Entity *SpawnDrill(int x, int y)
{
	Entity *drill;
	drill = NewEntity();
	if(drill == NULL)return drill;
	drill->sprite = LoadSprite("images/drill.png",32,30);
	drill->sound[0] = LoadSound("sounds/drill_death.wav", MIX_MAX_VOLUME/2);
	drill->sound[1] = LoadSound("sounds/drill_hit.wav", MIX_MAX_VOLUME/2);
	drill->bbox.x = 0;
	drill->bbox.y = 0;
	drill->bbox.w = 32;
	drill->bbox.h = 30;
	drill->frame = 0;
	drill->sx = x;
	drill->sy = y;
	drill->vx = -4;
	drill->vy = 0;
	drill->shown = 1;
	drill->layer = 0;
	drill->delay = 1;
	drill->busy = 0;
	drill->ct = 0;
	drill->health = 1;
	drill->healthmax = 1;
	drill->state = ST_ENEMY;
	drill->isRight = 0;
	drill->think = DrillThink;
	drill->owner = NULL;
	return drill;
}

void DrillThink(Entity *self)
{
	SDL_Rect b1, b2, b0, xCol, yCol;
	int uCheck2;
	float speed = 4;

	if(Player->sx + 300 > self->sx)self->ct = 1;

	if(self->health > 0 && self->ct)
	{
		/*Check for Collisions*/
		if(self->isRight)b0.x = self->sx + self->bbox.x + self->bbox.w;
		else b0.x = self->sx + self->bbox.x - self->bbox.w;
		b0.y = self->sy + self->bbox.y;
		b0.w = self->bbox.w;
		b0.h = self->bbox.h;
		self->state = ST_IDLE;
		CheckCollisions(self, b0, &xCol, &yCol, &empty);
		uCheck2 = self->uCheck;
	
		b1.x = self->sx + self->bbox.x;
		b1.y = self->sy + self->bbox.y;
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		b2.x = Player->sx + Player->bbox.x;
		b2.y = Player->sy + Player->bbox.y;
		b2.w = Player->bbox.w;
		b2.h = Player->bbox.h;
		self->state = ST_ENEMY;
		CheckCollisions(self, b1, &xCol, &yCol, &empty);

		/*What to Do if Colliding*/
		if(self->uCheck)
		{	
			if(self->sy > yCol.y - self->bbox.h)
				self->vy = -2;
			else 
			{
				self->sy = yCol.y - self->bbox.h;
				self->vy = 0;
				self->busy = 1;
			}
		}
		if(self->lCheck)
		{
			self->isRight = 0;
			self->vx = -1 * abs(self->vx);
		}
		if(self->rCheck)
		{
			self->isRight = 1;
			self->vx = abs(self->vx);
		}

		if(!uCheck2)		/*Stops self from going off edges*/
		{
			if(self->isRight)
			{
				self->isRight = 0;
				self->vx = -1 * abs(self->vx);
			}
			else 
			{
				self->isRight = 1;
				self->vx = abs(self->vx);
			}
		}
		
		if(self->isRight)		/*Move right*/
		{
			if(self->vx < speed)
				self->vx++;
			if(self->vx > speed)
				self->vx--;
		}
		else					/*Move left*/
		{
			if(self->vx > -speed)
				self->vx--;
			if(self->vx < -speed)
				self->vx++;
		}

		if(self->vy <= 13 && !self->uCheck)	/*Gravity*/
		{
			if(self->isRight)self->vx = 2;
			else self->vx = -2;
			self->vy += 2;
		}

		self->sx += self->vx * self->busy;
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

		if(self->invuln)
		{
			if(self->sy > Player->sy + Player->sprite->h)self->health--;
			if(self->sx > Player->sx)self->vx = 7;
			else self->vx = -7;
			//Mix_PlayChannel(-1,self->sound[1]->sound,0);
			self->invuln = 0;
		}
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			break;
		case ST_ENEMY:
			if(self->delay <= 0)
			{
				if(!self->busy)self->delay = 5;
				else self->delay = 2;
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

	if(self->sx + self->bbox.w < onset || self->sy > 800)FreeEntity(self);		/*When offscreen, free self*/
}

Entity *SpawnBall(int x, int y)
{
	Entity *ball;
	ball = NewEntity();
	if(ball == NULL)return ball;
	ball->sprite = LoadSprite("images/ball.png",32,32);
	ball->sound[0] = LoadSound("sounds/ball_death.wav", MIX_MAX_VOLUME/2);
	ball->sound[1] = LoadSound("sounds/ball_laser.wav", MIX_MAX_VOLUME/2);
	ball->bbox.x = 0;
	ball->bbox.y = 0;
	ball->bbox.w = 32;
	ball->bbox.h = 32;
	ball->frame = 0;
	ball->sx = x;
	ball->sy = y;
	ball->shown = 1;
	ball->isRight = 1;
	ball->busy = 0;
	ball->delay = 10;
	ball->health = 1;
	ball->healthmax = 1;
	ball->state = ST_ENEMY;
	ball->think = BallThink;
	ball->owner = NULL;
	return ball;
}

void BallThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	float px, py, pz, hyp;
	float speed = 10;
	
	if(self->health > 0)
	{	
		/*Check for Collisions*/
		b1.x = self->sx + self->bbox.x;
		b1.y = self->sy + self->bbox.y;
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		b2.x = Player->sx + Player->bbox.x;
		b2.y = Player->sy + Player->bbox.y;
		b2.w = Player->bbox.w;
		b2.h = Player->bbox.h;
		CheckCollisions(self, b1, &xCol, &yCol, &empty);
		
		if(self->state == ST_ENEMY)
		{
			if(!self->busy)
			{
				if(self->isRight)
				{
					px = (Player->sx + 300) - self->sx;
					py = Player->sy - self->sy;
					pz = sqrt((px * px) + (py * py));
					if(pz != 0)hyp = speed / pz;
					else hyp = 0;
					self->vx = px * hyp;
					self->vy = py * hyp;
					if(pz < 16)
					{
						self->busy = 1;
						self->delay = 10;
						self->frame = 1;
					}
				}
			}
			else
			{
				self->vx = 0;
				self->vy = 0;
				self->sx = Player->sx + 300;
				self->sy = Player->sy;
			}

			if(self->isRight)
			{
				self->sx += self->vx;
				self->sy += self->vy;
			}
		}
		else
		{
			if(self->busy == 0)
			{
				self->state = ST_ENEMY;
				self->delay = 0;
				self->frame = 4;
			}
			else self->busy--;
		}

		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)	/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
				Player->vx = -15;
			else Player->vx = 15;
			Player->invuln = 30;
		}

		if(self->invuln)self->health--;
	}
	
	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 6;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
	}

	switch(self->state)		/*Animations*/
	{
		case ST_DEAD:
			self->vy += 2;
			self->sy += self->vy;
			if(self->sy >= 800)FreeEntity(self);
			break;
		case ST_IDLE:
			if(self->delay < 0)								/*Fire*/
			{
				ShootLaser(self->sx - 32, self->sy + 12);
				Mix_PlayChannel(-1,self->sound[1]->sound,0);
				self->delay = 0;
				self->busy = 30;
			}
			if(self->delay == 0)
			{
				self->delay = 4;
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
		case ST_ENEMY:
			if(self->delay <= 0)
			{
				if(self->isRight)
				{
					if(!self->busy)							/*Approach*/
					{
						self->delay = 20;
						switch(self->frame)
						{
							case 0: self->frame = 1;
								break;
							case 1: self->frame = 0;
								break;
						}
					}
					else									/*Locked*/
					{
						self->delay = 15;
						if(self->frame < 3)self->frame++;
						else 
						{
							self->frame++;
							self->delay = -1;
							self->isRight = 0;
							self->state = ST_IDLE;
						}
					}
				}
				else										/*Cooldown*/
				{
					self->delay = 5;
					if(self->frame > 2)self->frame--;
					else 
					{
						self->frame--;
						self->delay = 20;
						self->busy = 0;
						self->isRight = 1;
					}
				}
			}
			else self->delay--;
			break;
	}
}

Entity *ShootLaser(int x, int y)
{
	Entity *laser;
	laser = NewEntity();
	if(laser == NULL)return laser;
	laser->sprite = LoadSprite("images/laser.png", 64, 8);
	laser->bbox.x = 0;
	laser->bbox.y = 0;
	laser->bbox.w = 64;
	laser->bbox.h = 8;
	laser->frame = 0;
	laser->sx = x;
	laser->sy = y;
	laser->vx = -20;
	laser->shown = 1;
	laser->delay = 2;
	laser->state = ST_ENEMY;
	laser->think = LaserThink;
	laser->owner = NULL;
	return laser;
}

void LaserThink(Entity *self)
{
	SDL_Rect b1, b2;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;

	if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)	/*Check for Player Collision*/
	{
		Player->health -= 1;
		Player->vy = -15;
		if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
			Player->vx = -15;
		else Player->vx = 15;
		Player->invuln = 30;
	}

	self->sx += self->vx;
	if(self->sx < onset)FreeEntity(self);
}

Entity *SpawnBoss()
{
	Entity *boss;
	boss = NewEntity();
	if(boss == NULL)return boss;
	boss->sprite = LoadSprite("images/bossbody.png", 72, 94);
	boss->sound[0] = LoadSound("sounds/boss_quake.wav", MIX_MAX_VOLUME/2);
	boss->shown = 1;
	boss->frame = 0;
	boss->bbox.x = 26;
	boss->bbox.y = 4;
	boss->bbox.w = 46;
	boss->bbox.h = 92;
	boss->sx = 792;
	boss->sy = 506;
	boss->isRight = 5;
	boss->health = 5;
	boss->healthmax = 5;
	boss->state = ST_ENEMY;
	boss->think = BossThink;
	boss->owner = NULL;
	Boss = boss;

	Entity *head1;
	head1 = NewEntity();
	if(head1 == NULL)return head1;
	head1->sprite = LoadSprite("images/bosshead.png", 34, 32);
	head1->sound[0] = LoadSound("sounds/boss_fireball.wav", MIX_MAX_VOLUME/2);
	head1->shown = 1;
	head1->frame = 0;
	head1->bbox.x = 2;
	head1->bbox.y = 0;
	head1->bbox.w = 32;
	head1->bbox.h = 32;
	head1->sx = boss->sx;
	head1->sy = boss->sy;
	head1->vx = 0.25;
	head1->isRight = 0;
	head1->busy = 0;
	head1->ct = 40;
	head1->health = boss->sx - 8;
	head1->healthmax = boss->sx + 8;
	head1->state = ST_ENEMY;
	head1->think = BossHeadThink1;
	head1->owner = boss;

	Entity *head2;
	head2 = NewEntity();
	if(head2 == NULL)return head2;
	head2->sprite = LoadSprite("images/bosshead.png", 34, 32);
	head2->sound[0] = LoadSound("sounds/boss_firewall.wav", MIX_MAX_VOLUME/2);
	head2->shown = 1;
	head2->frame = 0;
	head2->bbox.x = 2;
	head2->bbox.y = 0;
	head2->bbox.w = 32;
	head2->bbox.h = 32;
	head2->sx = boss->sx;
	head2->sy = boss->sy + 32;
	head2->vx = 0.1875;
	head2->isRight = 0;
	head2->busy = 0;
	head2->ct = 20;
	head2->health = boss->sx - 6;
	head2->healthmax = boss->sx + 6;
	head2->state = ST_ENEMY;
	head2->think = BossHeadThink2;
	head2->owner = boss;

	return boss;
}

void BossThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);
	
	if(self->health > 0)	/*While Alive*/
	{
		if(self->invuln > 0)	/*When hit*/
		{
			if(self->invuln == 63)
			{
				Mix_PlayChannel(-1,self->sound[0]->sound,0);
				self->health--;
			}

			if(self->invuln >= 60)self->frame = 1;			/*Tail slam*/
			else self->frame = 0;

			if(self->invuln >= 50 && self->invuln < 60)		/*Player sent back*/
			{
				Player->invuln = 20;
				Player->vx = -35;
				Player->vy = -10;
			}

			if(self->invuln < 60)							/*Screen shake*/
			{
				if(quake < 1)quake = self->invuln / 10;
				else quake = -self->invuln / 10;
			}
			
			self->invuln--;
		}
		else
		{
			quake = 0;										/*Stop screen*/
		}

		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)		/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			Player->vx = -15;
			Player->invuln = 30;
		}
	}
	else
	{
		if(self->state != ST_DEAD)		/*Dying*/
		{
			self->vy = -15;
			self->state = ST_DEAD;
			self->frame = 2;
		}
	}

	if(self->state == ST_DEAD)			/*Dead*/
	{
		self->vy += 1;
		self->sy += self->vy;
		//if(self->sy >= 800)FreeEntity(self);
	}
}

void BossHeadThink1(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;

	self->invuln = 0;

	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(Boss->health > 0)	/*While Alive*/
	{
		if(!self->busy)
		{
			if(self->delay > 0)		/*Head Movement*/
			{
				if(self->isRight && self->sx < self->healthmax)
				{
					self->sx += self->vx;			
				}	
				else if(self->sx > self->health) self->sx -= self->vx;

				self->delay--;
			}
			else
			{
				self->delay = rand() % 32;
				if(self->isRight)self->isRight = 0;
				else self->isRight = 1;
			}

			if(self->ct >= 0)self->ct--;		/*Countdown to Attack*/
			else
			{
				self->busy = -1;
				self->ct = -6 * (6 - Boss->health);
			}
		}

		if(self->invuln && self->busy <= 0 && Boss->invuln == 0)		/*Hit*/
		{
			Boss->invuln = 63;
			self->busy = 60;
			self->frame = 2;
			self->invuln = 0;
		}

		if(self->busy > 0)		/*Post-hit Invincibility*/
		{
			if(self->busy < 10)self->frame = 0;
			self->ct = 0;
			self->busy--;
		}

		if(self->busy < 0)		/*Attack*/
		{
			if(self->ct < 0)
			{
				self->frame = 1;
				if(self->ct % 10 == -1)
				{
					FireBall(self->sx - 12, self->sy + 12);
					Mix_PlayChannel(-1,self->sound[0]->sound,0);
				}
				if(self->ct == -1)self->ct = 30;

				self->ct++;
			}
			if(self->ct > 0)self->ct--;
			else if(self->ct == 0)
				{
					self->frame = 0;
					self->busy = 0;
					self->ct = 30 + (Boss->health * 10);
				}
		}
		
		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)		/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			Player->vx = -15;
			Player->invuln = 30;
		}
	}
	else FreeEntity(self);	/*Dead*/
}

void BossHeadThink2(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;

	self->invuln = 0;

	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty);

	if(Boss->health > 0)	/*While Alive*/
	{
		if(!self->busy)
		{
			if(self->delay > 0)		/*Head Movement*/
			{
				if(self->isRight && self->sx < self->healthmax)
				{
					self->sx += self->vx;			
				}	
				else if(self->sx > self->health) self->sx -= self->vx;

				self->delay--;
			}
			else
			{
				self->delay = rand() % 24;
				if(self->isRight)self->isRight = 0;
				else self->isRight = 1;
			}

			if(self->ct >= 0)self->ct--;		/*Countdown to Attack*/
			else
			{
				self->busy = -1;
				self->ct = -1;
			}
		}

		if(self->invuln && self->busy <= 0 && Boss->invuln == 0)		/*Hit*/
		{
			Boss->invuln = 63;
			self->busy = 60;
			self->frame = 2;
			self->invuln = 0;
		}

		if(self->busy > 0)		/*Post-hit Invincibility*/
		{
			if(self->busy < 10)self->frame = 0;
			self->invuln = 0;
			self->ct = 0;
			self->busy--;
		}

		if(self->busy < 0)		/*Attack*/
		{
			if(self->ct < 0)
			{
				self->frame = 1;
				FireWall(self->sx - 11, self->sy - 34, 0);
				Mix_PlayChannel(-1,self->sound[0]->sound,0);
				self->ct = 30;
			}
			if(self->ct > 0)self->ct--;
			else if(self->ct == 0)
				{
					self->frame = 0;
					self->busy = 0;
					if(Boss->health >= 3)self->ct = 50;
					else self->ct = 40;
				}
		}
		
		if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0)		/*Check for Player Collision*/
		{
			Player->health -= 1;
			Player->vy = -15;
			Player->vx = -15;
			Player->invuln = 30;
		}
	}
	else FreeEntity(self);	/*Dead*/
}

Entity *FireBall(int x, int y)
{
	Entity *fire;
	fire = NewEntity();
	if(fire == NULL)return fire;
	fire->sprite = LoadSprite("images/fireball.png", 32, 32);
	fire->bbox.x = 4;
	fire->bbox.y = 4;
	fire->bbox.w = 24;
	fire->bbox.h = 24;
	fire->frame = 0;
	fire->sx = x;
	fire->sy = y;
	fire->shown = 1;
	fire->ct = 0;
	fire->delay = 0;
	fire->state = ST_ENEMY;
	fire->think = FireBallThink;
	fire->owner = NULL;
	return fire;
}

void FireBallThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	float px, py, pz, hyp;
	float speed = 7;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty); 

	if(self->ct == 0)
	{
		px = Player->sx - self->sx;
		py = Player->sy - self->sy;
		pz = sqrt((px * px) + (py * py));
		if(pz != 0)hyp = speed / pz;
		else hyp = 0;
		self->vx = px * hyp;
		self->vy = py * hyp;

		self->ct++;
	}
	
	if(self->state == ST_ENEMY)
	{
		self->sx += self->vx;
		self->sy += self->vy;
	}

	if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0 && self->state == ST_ENEMY)	/*Check for Player Collision*/
	{
		Player->health -= 1;
		Player->vy = -15;
		if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
			Player->vx = -15;
		else Player->vx = 15;
		Player->invuln = 30;
	}

	if(self->uCheck || Boss->invuln > 50)self->state = ST_DYING;	/*Disappear During Quake*/

	if(self->state == ST_DYING)
	{
		if(self->delay > 0)self->delay--;
		else
		{
			if(self->frame < 5)self->frame++;
			else FreeEntity(self);

			self->delay = 4;
		}
	}

	if(self->sx < onset)FreeEntity(self);
}

Entity *FireWall(int x, int y, int i)
{
	Entity *fire;
	fire = NewEntity();
	if(fire == NULL)return fire;
	fire->sprite = LoadSprite("images/firewall.png", 32, 64);
	fire->bbox.x = 11;
	fire->bbox.y = 53;
	fire->bbox.w = 13;
	fire->bbox.h = 11;
	fire->frame = i;
	fire->sx = x;
	fire->sy = y;
	fire->vx = -2;
	fire->vy = 2;
	fire->shown = 1;
	fire->ct = 0;
	fire->delay = 0;
	fire->isRight = 1;
	fire->state = ST_ENEMY;
	fire->think = FireWallThink;
	fire->owner = NULL;
	return fire;
}

void FireWallThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol, &empty); 
	
	if(self->frame == 0)
	{
		self->sx += self->vx;
		self->sy += self->vy;
	}

	if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0 && self->state == ST_ENEMY)	/*Check for Player Collision*/
	{
		Player->health -= 1;
		Player->vy = -15;
		if((Player->sx + Player->bbox.x + (Player->bbox.w / 2)) < (self->sx + self->bbox.x + (self->bbox.w / 2)))
			Player->vx = -15;
		else Player->vx = 15;
		Player->invuln = 30;
	}

	if(self->uCheck && self->state != ST_DYING)	/*Hit Ground*/
	{
		if(self->delay > 0)self->delay--;
		else
		{
			if(self->isRight)
			{
				switch(self->frame)		/*Rise*/
				{
					case 0:
						self->frame++;
						self->bbox.x = 1;
						self->bbox.y = 52;
						self->bbox.w = 30;
						self->bbox.h = 12;
						self->sy = yCol.y - 64;
						break;
					case 1:
						self->frame++;
						self->bbox.y = 36;
						self->bbox.h = 28;
						break;
					case 2:
						self->frame++;
						self->bbox.y = 20;
						self->bbox.h = 44;
						
						break;
					case 3:
						self->frame++;
						self->bbox.y = 4;
						self->bbox.h = 60;
						if(self->sx > 128)
						{
							FireWall(self->sx - 32, self->sy, 1);
							//Mix_PlayChannel(-1,self->sound[0]->sound,0);
						}
						break;
					case 4:
						self->isRight = 0;
						break;
				}
			}
			else						/*Fall*/
			{
				switch(self->frame)
				{
					case 4:
						self->frame--;
						self->bbox.y = 20;
						self->bbox.h = 44;
						break;
					case 3:
						self->frame--;
						self->bbox.y = 36;
						self->bbox.h = 28;
						break;
					case 2:
						self->frame--;
						self->bbox.y = 52;
						self->bbox.h = 12;
						break;
					case 1:
						FreeEntity(self);
						break;
				}
			}

			if(Boss->health > 3)self->delay = 3;
			else self->delay = 2;
		}
	}
		
	if(Boss->invuln > 50)self->state = ST_DYING;	/*Disappear During Quake*/

	if(self->state == ST_DYING)
	{
		if(self->delay > 0)self->delay--;
		else
		{
			if(self->frame == 0)self->frame++;
			if(self->frame < 9)self->frame += 4;
			else FreeEntity(self);

			self->delay = 5;
		}
	}
}

Entity *BuildTile(int x)
{
	Entity *tile;
	tile = NewEntity();
	if(tile == NULL)return tile;
	tile->sprite = LoadSprite("images/tile.png", 256, 32);
	tile->shown = 1;
	tile->sx = x;
	tile->sy = 600;
	tile->bbox.x = 0;
	tile->bbox.y = 0;
	tile->bbox.w = 256;
	tile->bbox.h = 32;
	tile->state = ST_TILE;
	tile->think = TileThink;
	tile->owner = NULL;
	return tile;
}

Entity *BuildSmallTile(int x)
{
	Entity *tile;
	tile = NewEntity();
	if(tile == NULL)return tile;
	tile->sprite = LoadSprite("images/small_tile.png", 64, 32);
	tile->shown = 1;
	tile->sx = x;
	tile->sy = 600;
	tile->bbox.x = 0;
	tile->bbox.y = 0;
	tile->bbox.w = 64;
	tile->bbox.h = 32;
	tile->state = ST_TILE;
	tile->think = TileThink;
	tile->owner = NULL;
	return tile;
}

Entity *BuildPlatform(int x, int y)
{
	Entity *plat;
	plat = NewEntity();
	if(plat == NULL)return plat;
	plat->sprite = LoadSprite("images/platform.png", 256, 12);
	plat->shown = 1;
	plat->sx = x;
	plat->sy = y;
	plat->bbox.x = 0;
	plat->bbox.y = 0;
	plat->bbox.w = 256;
	plat->bbox.h = 12;
	plat->state = ST_TILE;
	plat->think = TileThink;
	plat->owner = NULL;
	return plat;
}

Entity *BuildWall(int x, int y)
{
	Entity *wall;
	wall = NewEntity();
	if(wall == NULL)return wall;
	wall->sprite = LoadSprite("images/wall.png", 30, 256);
	wall->shown = 1;
	wall->sx = x;
	wall->sy = y;
	wall->bbox.x = 0;
	wall->bbox.y = 0;
	wall->bbox.w = 30;
	wall->bbox.h = 256;
	wall->state = ST_TILE;
	wall->think = TileThink;
	wall->owner = NULL;
	return wall;
}

Entity *BuildColumn(int x, int y)
{
	Entity *column;
	column = NewEntity();
	if(column == NULL)return column;
	column->sprite = LoadSprite("images/column.png", 64, 64);
	column->shown = 1;
	column->sx = x;
	column->sy = y;
	column->bbox.x = 0;
	column->bbox.y = 0;
	column->bbox.w = 64;
	column->bbox.h = 64;
	column->state = ST_TILE;
	column->think = TileThink;
	column->owner = NULL;
	return column;
}

Entity *BuildCloudPlatform(int x, int y)
{
	Entity *plat;
	plat = NewEntity();
	if(plat == NULL)return plat;
	plat->sprite = LoadSprite("images/cloudplatform.png", 192, 8);
	plat->shown = 1;
	plat->sx = x;
	plat->sy = y;
	plat->bbox.x = 0;
	plat->bbox.y = 4;
	plat->bbox.w = 192;
	plat->bbox.h = 4;
	plat->invuln = 1;			/*Can be jumped through*/
	plat->state = ST_TILE;
	plat->think = TileThink;
	plat->owner = NULL;
	return plat;
}

void TileThink(Entity *self)
{
	if(self->sx + self->bbox.w < onset)FreeEntity(self);
}

Entity *BuildMovingPlatform(int x, int y, int a, int b)
{
	Entity *plat;
	plat = NewEntity();
	if(plat == NULL)return plat;
	plat->sprite = LoadSprite("images/movingplatform.png", 64, 16);
	plat->shown = 1;
	plat->sx = x;
	plat->sy = y;
	plat->bbox.x = 0;
	plat->bbox.y = 0;
	plat->bbox.w = 64;
	plat->bbox.h = 16;
	plat->uCheck = x;			/*Point 1's x position*/
	plat->dCheck = y;			/*Point 1's y position*/
	plat->lCheck = a;			/*Point 2's x position*/
	plat->rCheck = b;			/*Point 2's y position*/
	plat->isRight = 1;
	plat->state = ST_TILE;
	plat->think = PlatThink;
	plat->owner = NULL;
	return plat;
}

void PlatThink(Entity *self)
{
	SDL_Rect b1, b2;
	float px, py, pz, hyp;
	float speed = 2;

	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	
	if(self->isRight)
	{
		px = self->lCheck - self->sx;
		py = self->rCheck - self->sy;
	}
	else
	{
		px = self->uCheck - self->sx;
		py = self->dCheck - self->sy;
	}

	pz = sqrt((px * px) + (py * py));
	if(pz != 0)hyp = speed / pz;
	else hyp = 0;
	self->vx = px * hyp;
	self->vy = py * hyp;
	if(pz < 5)
		if(self->isRight)
			self->isRight = 0;
		else
			self->isRight = 1;

	self->sx += self->vx;
	self->sy += self->vy;

	if(Collide(b1,b2) && Player->uCheck)
		Player->sx += self->vx;
	
	if(self->sx + self->bbox.w < onset)FreeEntity(self);
}

Entity *BuildSpring(int x, int y)
{
	Entity *spring;
	spring = NewEntity();
	if(spring == NULL)return spring;
	spring->sprite = LoadSprite("images/spring.png", 48, 48);
	spring->sound[0] = LoadSound("sounds/spring.wav", MIX_MAX_VOLUME/2);
	spring->shown = 1;
	spring->sx = x;
	spring->sy = y;
	spring->bbox.x = 0;
	spring->bbox.y = 12;
	spring->bbox.w = 48;
	spring->bbox.h = 36;
	spring->state = ST_TILE;
	spring->think = SpringThink;
	spring->owner = NULL;
	return spring;
}

void SpringThink(Entity *self)
{
	SDL_Rect b1, b2;
	
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	
	if(Collide(b1,b2) && Player->uCheck && b2.y + b2.h <= self->sy + self->bbox.y)
	{
		Player->vy = -30;
		Mix_PlayChannel(-1,self->sound[0]->sound,0);
		self->frame = 1;
		self->delay = 5;
	}

	if(self->delay > 0)self->delay--;
	else self->frame = 0;

	if(self->sx + self->bbox.w < onset)FreeEntity(self);
}

Entity *BuildSpikeWall(int x, int a, int i)
{
	Entity *wall;
	wall = NewEntity();
	if(wall == NULL)return wall;
	wall->sprite = LoadSprite("images/spikewall.png", 32, 720);
	wall->shown = 1;
	wall->sx = x;
	wall->sy = 720;
	wall->vx = i;
	wall->vy = 20;
	wall->bbox.x = 0;
	wall->bbox.y = 0;
	wall->bbox.w = 16;
	wall->bbox.h = 720;
	wall->busy = a;
	wall->delay = 0;
	wall->state = ST_TILE;
	wall->think = WallThink;
	wall->owner = NULL;
	return wall;
}

void WallThink(Entity *self)
{
	SDL_Rect b1, b2;
	
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w + 16;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	
	if(Player->sx >= self->sx + 100)self->delay = 1;
	if(self->delay && self->sy > 0)
	{
		self->sy -= self->vy;
		if(self->sy < 0)self->sy = 0;
	}
	if(self->sy == 0 && self->sx <= self->busy)
	{
		self->sx += self->vx;
	}

	if(Collide(b1,b2))	/*Check for Player Collision*/
	{
		Player->health -= 5;
		Player->vy = -15;
		Player->vx = 15;
		Player->invuln = 30;
	}
}

Entity *BuildSpikes(int x, int y, int i, int j)
{
	Entity *spike;
	spike = NewEntity();
	if(spike == NULL)return spike;
	spike->sprite = LoadSprite("images/spikes.png", 64, 41);
	spike->shown = 1;
	spike->sx = x;
	spike->sy = y;
	spike->bbox.x = 3;
	spike->bbox.y = 32;
	spike->bbox.w = 58;
	spike->bbox.h = 32;
	spike->busy = i;			//Time between intervals
	spike->delay = i + j;		//Offset
	spike->isRight = 1;
	spike->state = ST_TILE;
	spike->think = SpikeThink;
	spike->owner = NULL;
	return spike;
}

void SpikeThink(Entity *self)
{
	SDL_Rect b1, b2;

	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;

	if(Collide(b1,b2) && Player->invuln == 0 && Player->health > 0 && (self->frame == 2 || self->frame == 3))	/*Check for Player Collision*/
	{
		Player->health -= 1;
		Player->vy = -15;
		Player->vx = -10;
		Player->invuln = 30;
	}

	switch(self->frame)
	{
		case 0:
			if(self->delay > 15)self->delay--;
			else self->frame++;
			break;
		case 1:
			if(self->delay > 0)self->delay--;
			else 
			{
				self->frame++;
				self->delay = 2;
				self->bbox.y = 16;
				//self->bbox.h = 96;
				self->isRight = 1;
			}
			break;
		case 2:
			if(self->delay > 0)self->delay--;
			else
			{
				if(self->isRight)
				{
					self->frame++;
					self->delay = 10;
					self->bbox.y = 0;
					//self->bbox.h = 96;
				}
				else
				{
					self->frame = 0;
					self->delay = self->busy;
					self->bbox.y = 32;
					//self->bbox.h = 64;
				}
			}
			break;
		case 3:
			if(self->delay > 0)self->delay--;
			else
			{
				self->frame--;
				self->delay = 3;
				self->bbox.y = 16;
				//self->bbox.h = 80;
				self->isRight = 0;
			}
			break;
	}
	
	if(self->sx + self->bbox.w < onset)FreeEntity(self);
}

Entity *BuildFlag(int x, int y, int i)
{
	Entity *flag;
	flag = NewEntity();
	if(flag == NULL)return flag;
	if(i == 1)
	{
		flag->sprite = LoadSprite("images/flag1.png", 24, 48);
		flag->think = NULL;
	}
	else
	{
		flag->sprite = LoadSprite("images/flag2.png", 24, 48);
		flag->think = FlagThink;
	}
	flag->shown = 1;
	flag->layer = 0;
	if(Player->sx < x)flag->frame = 0;
	else flag->frame = 4;
	flag->sx = x;
	flag->sy = y;
	flag->delay = 0;
	flag->ct = 0;
	flag->owner = NULL;
	return flag;
}

void FlagThink(Entity *self)
{
	if(Player->sx >= self->sx)self->ct = 1;

	if(self->frame < 4 && self->ct)
	{
		if(self->delay > 0)self->delay--;
		else
		{
			self->frame++;
			self->delay = 3;
		}
	}
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
