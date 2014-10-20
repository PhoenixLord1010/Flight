#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
extern SDL_Rect Camera;
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
		if(EntityList[i].used != 0)
		{
			if(EntityList[i].shown != 0)
				DrawEntity(&EntityList[i]);
		}
	}
}

void UpdateEntities()
{
	int i;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used != 0)
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
		if(EntityList[i].sprite != NULL)FreeSprite(EntityList[i].sprite);
		EntityList[i].sprite = NULL;
		EntityList[i].owner = NULL;
		EntityList[i].think = NULL;
		EntityList[i].shown = 0;
		EntityList[i].used = 0;
	}
	memset(EntityList, 0, sizeof(Entity) * MAXENTITIES);
}

Entity *NewEntity()
{
	int i;
	if(NumEnts + 1 >= MAXENTITIES)
		return NULL;
	for(i = 0; i <= MAXENTITIES; i++)
	{
		if(EntityList[i].used == 0)
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
	ent->sprite = NULL;
	ent->owner = NULL;
	ent->think = NULL;
	ent->shown = 0;
	ent->state = 0;
	memset(ent, 0, sizeof(Entity));
}

void ClearEntities()
{
	int i = 0;
	for(i = 0; i < MAXENTITIES; i++)
	{
		if(EntityList[i].used != 0)
			FreeEntity(&EntityList[i]);
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
	player->busy = 0;
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
	
	/*Spear Stuff*/
	int sFrame;
	SDL_Rect sBox;

	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y + self->vy/1.5;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol);

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
			self->sx = xCol.x - 32;
		}
		if(self->rCheck)
		{
			self->sx = xCol.x + xCol.w;
		}
		if(self->sx < screen->offset)self->sx = screen->offset;


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
			if(isKeyHeld(SDLK_a) && !self->rCheck)					/*Move left*/
			{
				if(self->busy <= 0)self->isRight = 0;
				if(self->vx > -8)self->vx -= 4;
				if(self->uCheck && self->state != ST_DASH)self->state = ST_WALK;
			}
			if(isKeyHeld(SDLK_d) && !self->lCheck)					/*Move right*/
			{
				if(self->busy <= 0)self->isRight = 1;
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

		if((isKeyPressed(SDLK_KP6) || isKeyPressed(SDLK_KP5)) && self->busy <= 0)	/*Right*/
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
	return spear;
}

void SpearThink(Entity *self)
{
	self->sx = Player->sx - 23;
	self->sy = Player->sy - 20;

	/*Directional Attack*/
	if((isKeyPressed(SDLK_KP6) || isKeyPressed(SDLK_KP5)) && self->busy <= 0)	/*Right*/
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

	if(self->busy >= 0)self->busy--;
	else 
	{
		self->shown = 0;
		self->state = ST_IDLE;
	}
}


Entity *SpawnSnake(int x, int y)
{
	Entity *snake;
	snake = NewEntity();
	if(snake == NULL)return snake;
	snake->sprite = LoadSprite("images/snake.png",20,32);
	snake->bbox.x = 2;
	snake->bbox.y = 0;
	snake->bbox.w = 16;
	snake->bbox.h = 32;
	snake->frame = 0;
	snake->sx = x;
	snake->sy = y;
	snake->vx = -3;
	snake->vy = 0;
	snake->shown = 1;
	snake->delay = 1;
	snake->health = 1;
	snake->healthmax = 1;
	snake->state = ST_ENEMY;
	snake->isRight = 0;
	snake->think = SnakeThink;
	snake->owner = NULL;
	return snake;
}

void SnakeThink(Entity *self)
{
	SDL_Rect b1, b2, xCol, yCol;
	
	/*Check for Collisions*/
	b1.x = self->sx + self->bbox.x;
	b1.y = self->sy + self->bbox.y + self->vy/1.5;
	b1.w = self->bbox.w;
	b1.h = self->bbox.h;
	b2.x = Player->sx + Player->bbox.x;
	b2.y = Player->sy + Player->bbox.y;
	b2.w = Player->bbox.w;
	b2.h = Player->bbox.h;
	CheckCollisions(self, b1, &xCol, &yCol);

	if(self->isRight)self->frame -= 3;

	if(self->health > 0)
	{
		if(self->vy <= gravity && !self->uCheck)	/*Gravity*/
			self->vy += 2;

		/*What to Do if Colliding*/
		if(self->uCheck)
		{
			self->vy = 0;
			self->sy = yCol.y - self->bbox.h;
		}
		if(self->lCheck)
		{
			self->vx = -1 * abs(self->vx);
		}
		if(self->rCheck)
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

	if(self->sx + self->bbox.w < screen->offset)FreeEntity(self);
}

Entity *SpawnEye(int x, int y)
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
	eye->delay = 1;
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

	if(self->health > 0)
	{	
		self->vy = 2 * sin(float(self->sx / 100));
		
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

	if(self->sx + self->bbox.w < screen->offset)FreeEntity(self);	/*Off Screen*/

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
	return tile;
}

Entity *BuildPlatform(int x, int y)
{
	Entity *plat;
	plat = NewEntity();
	if(plat == NULL)return plat;
	plat->sprite = LoadSprite("images/platform.png", 128, 12);
	plat->shown = 1;
	plat->sx = x;
	plat->sy = y;
	plat->bbox.x = 0;
	plat->bbox.y = 0;
	plat->bbox.w = 128;
	plat->bbox.h = 12;
	plat->state = ST_TILE;
	plat->think = TileThink;
	return plat;
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
	return column;
}

void TileThink(Entity *self)
{
	if(self->sx + self->bbox.w < screen->offset)FreeEntity(self);
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
