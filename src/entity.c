#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
extern SDL_Rect Camera;
extern float offset;

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


void InitEntityList()
{
	int i;
	NumEnts = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		EntityList[i].used = 0;
		EntityList[i].think = NULL;
		if(EntityList[i].sprite != NULL)FreeSprite(EntityList[i].sprite);
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
			EntityList[i].shown = 1;
			NumEnts++;
			return &EntityList[i];
		}
	}
	return NULL;
}

void FreeEntity(Entity *ent)
{
	NumEnts--;
	ent->used = 0;
	if(ent->sprite != NULL)FreeSprite(ent->sprite);
	memset(ent, 0, sizeof(Entity));
}

void DrawEntity(Entity *ent)
{
	DrawSprite(ent->sprite, screen, ent->sx, ent->sy, ent->frame);
}


void DrawEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if((EntityList[i].used > 0) && (EntityList[i].shown > 0))
			DrawEntity(&EntityList[i]);
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

void CheckCollisions(Entity *self, SDL_Rect box1, SDL_Rect *xCollision, SDL_Rect *yCollision)
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
				if((abs((box1.y + box1.h) - box2.y) <= abs((box1.x + box1.w) - box2.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.y + box1.h) - box2.y) <= abs((box2.y + box2.h) - box1.y)))
				{
					self->uCheck = 1;
					*yCollision = box2;
				}
				else if((abs((box1.x + box1.w) - box2.x) <= abs((box2.x + box2.w) - box1.x)) && (abs((box1.x + box1.w) - box2.x) <= abs((box2.y + box2.h) - box1.y)))
					 {	 
						 self->lCheck = 1;
						 *xCollision = box2;
					 }
					 else if((abs((box2.x + box2.w) - box1.x) <= abs((box2.y + box2.h) - box1.y)))
						  {	  
							  self->rCheck = 1;
							  *xCollision = box2;
						  }
						  else 
						  {
							  self->dCheck = 1;
							  *yCollision = box2;
						  }
			}

			if(self->state == ST_ENEMY && EntityList[i].state == ST_SPEAR)self->health--;	/*Enemy was hit*/
		}
	}
}


Entity *MakePlayer(int x, int y)
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
	player->sx = x;
	player->sy = y;
	player->shown = 1;
	player->delay = 1;
	player->busy = 0;
	player->state = ST_IDLE;
	player->isRight = 1;
	player->think = PlayerThink;
	player->health = 5;
	player->healthmax = 5;
	player->invuln = 0;
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
	
	/*Spear Stuff*/
	int sFrame;
	SDL_Rect sBox;

	
	/*Check for Collisions*/
	if(self->state == ST_DASH)
	{
		do
		{
			b1.x = self->sx + self->bbox.x + (self->vx * (i / 60));
			b1.y = self->sy + self->bbox.y + self->vy/1.5;
			b1.w = self->bbox.w;
			b1.h = self->bbox.h;
			CheckCollisions(self, b1, &xCol, &yCol);
			i++;
		}
		while((i < 61) && (self->lCheck != 1 && self->rCheck != 1));
	}
	else 
	{
		b1.x = self->sx + self->bbox.x;
		b1.y = self->sy + self->bbox.y + self->vy/1.5;
		b1.w = self->bbox.w;
		b1.h = self->bbox.h;
		CheckCollisions(self, b1, &xCol, &yCol);
	}

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
			self->vy = 0;
			self->sy = yCol.y - self->bbox.h;
		}
		if(self->dCheck)
		{
			self->vy = 1;
			self->sy = yCol.y + yCol.h;
		}
		if(self->lCheck)
		{
			self->vx = 0;
			self->sx = xCol.x - 32;
		}
		if(self->rCheck)
		{
			self->vx = 0;
			self->sx = xCol.x + xCol.w;
		}
		
		/*Keep player on screen*/
		if(self->sx < offset)self->sx = offset;
		if(self->sx + self->sprite->w > screen->w + offset)self->sx = screen->w + offset - self->sprite->w;
		if(self->sy <= 0)self->vy = 1;


		self->sx += self->vx;
		self->sy += self->vy;

		if((self->vx == 0) && (self->vy == 0) && self->uCheck)	/*Standing Still*/
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
			if(isKeyHeld(SDLK_e) && !self->rCheck && self->state != ST_DASH)					/*Move left*/
			{
				if(self->busy <= 7)self->isRight = 0;
				if(self->vx > -8)self->vx -= 4;
				if(self->uCheck && self->state != ST_DASH)self->state = ST_WALK;
			}
			if(isKeyHeld(SDLK_t) && !self->lCheck && self->state != ST_DASH)					/*Move right*/
			{
				if(self->busy <= 7)self->isRight = 1;
				if(self->vx < 8)self->vx += 4;
				if(self->uCheck && self->state != ST_DASH)self->state = ST_WALK;
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
				self->uCheck = 0;
			}
		
			if(isKeyHeld(SDLK_KP0) && self->vy > 0)gravity = 1.5;		/*Float*/
			else gravity = 13;
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

		if((isKeyPressed(SDLK_KP_PLUS) || isKeyPressed(SDLK_KP_ENTER)) && self->busy <= 0 && Player->uCheck == 1)	/*Dash*/
		{
			if(self->isRight)
				Player->vx = 30;
			else 
				Player->vx = -30;
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
			if(self->busy == 1)self->state = ST_IDLE;
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
	if(isKeyPressed(SDLK_KP6) && self->busy <= 0)	/*Right*/
	{
		self->bbox.x = 68;
		self->bbox.y = 38;
		self->frame = 0;
		self->shown = 1;
		self->state = ST_SPEAR;
		Player->isRight = 1;
		self->busy = 15;
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
	}
	if(isKeyPressed(SDLK_KP8) && self->busy <= 0)	/*Up*/
	{
		self->bbox.x = 34;
		self->bbox.y = 0;
		self->frame = 6;
		self->shown = 1;
		self->state = ST_SPEAR;
		self->busy = 15;
	}
	if(isKeyPressed(SDLK_KP2) && self->busy <= 0)	/*Down*/
	{
		self->bbox.x = 34;
		self->bbox.y = 76;
		self->frame = 7;
		self->shown = 1;
		self->state = ST_SPEAR;
		self->busy = 15;
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


Entity *SpawnSnake(int x, int y, int i)
{
	Entity *snake;
	snake = NewEntity();
	if(snake == NULL)return snake;
	snake->sprite = LoadSprite("images/snake.png",20,32);
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
	CheckCollisions(self, b1, &xCol, &yCol);

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
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 2 + (3 * self->isRight);
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

	if(self->sx + self->bbox.w < offset || self->sy > 800)FreeEntity(self);		/*When offscreen, free self*/
}

Entity *SpawnEye(int x, int y, int wave)
{
	Entity *eye;
	eye = NewEntity();
	if(eye == NULL)return eye;
	eye->sprite = LoadSprite("images/eye.png",32,32);
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
	CheckCollisions(self, b1, &xCol, &yCol);

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
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 1;
	}

	if(self->delay > 0)self->delay--;

	if(self->sx + self->bbox.w < offset)FreeEntity(self);	/*Off Screen*/

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
	CheckCollisions(self, b1, &xCol, &yCol);

	if(self->health > 0)
	{	
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
	}
	
	if((self->health <= 0) && (self->state != ST_DEAD))	/*It died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = -20;
		self->state = ST_DEAD;
		self->frame = 6;
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
	pixie->bbox.x = 0;
	pixie->bbox.y = 0;
	pixie->bbox.w = 32;
	pixie->bbox.h = 32;
	pixie->frame = 0;
	pixie->sx = x;
	pixie->sy = Player->sy - 600;
	pixie->vx = -2;
	pixie->vy = 20;
	pixie->shown = 1;
	pixie->delay = 10;
	pixie->busy = Player->sy;	/*How far down to go*/
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
	CheckCollisions(self, b1, &xCol, &yCol);

	if(self->health > 0)
	{	
		if(self->sy <= self->busy)
		{
			self->vy -= 0.3;
		}
		else
		{
			self->vx = -20;
			self->vy = 0;
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
	wall->sprite = LoadSprite("images/wall.png", 24, 256);
	wall->shown = 1;
	wall->sx = x;
	wall->sy = y;
	wall->bbox.x = 0;
	wall->bbox.y = 0;
	wall->bbox.w = 24;
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

void TileThink(Entity *self)
{
	if(self->sx + self->bbox.w < offset)FreeEntity(self);
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
