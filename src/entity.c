#include <stdlib.h>
#include <math.h>
#include "entity.h"

extern SDL_Surface *screen;
Entity EntityList[MAXENTITIES];
Entity *Player;
int NumLives = 3;
int NumEnts;
int MOUSEMOVE = 1;
int lastx, lasty;


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

void CheckCollisions(SDL_Rect box1, int *up, int *down, int *left, int *right, SDL_Rect *xCollision, SDL_Rect *yCollision)
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
			if(EntityList[i].state == ST_TILE)
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
		}
	}
}


Entity *MakePlayer()
{
	Entity *player;
	player = NewEntity();
	if(player == NULL)return player;
	player->sprite = LoadSprite("images/player.png",32,48,-1,-1,-1);
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
	player->think = PlayerThink;
	player->health = 30;
	player->healthmax = 30;
	player->owner = player;
	Player = player;
	return player;
}

void PlayerThink(Entity *self)
{
	int numkeys;
	Uint8 *keys = SDL_GetKeyState(&numkeys);
	SDL_Rect b1, xCol, yCol;
	int uCheck, dCheck, lCheck, rCheck;

	b1.x = Player->sx + Player->bbox.x;
	b1.y = Player->sy + Player->bbox.y + Player->vy/1.5;
	b1.w = Player->bbox.w;
	b1.h = Player->bbox.h;
	CheckCollisions(b1, &uCheck, &dCheck, &lCheck, &rCheck, &xCol, &yCol);

	if(self->health > 0)
	{
		
		if(self->vx != 0)	/*decelerates x movement when nothing's pressed*/
		{
			if(self->vx > 0)
			{
				if(self->vx >= 3)
					self->vx -= 3;
				else self->vx = 0;
			}
			else
			{
				if(self->vx <= -3)
					self->vx += 3;
				else self->vx = 0;
			}
		}
		
		if(self->vy <= 13 && !uCheck)	/*gravity*/
			self->vy += 2;

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
	}

	if((self->health <= 0) && (self->state != ST_DEAD))	/*You died*/
	{
		self->health = 0;
		self->vx = 0;
		self->vy = 0;
		self->state = ST_DEAD;
		self->frame = 8;
	}
	
	if(keys[SDLK_a] && !rCheck && self->sx > 0)		/*Move left*/
	{
		if(self->vx > -10)self->vx -= 5;
		if(uCheck)self->state = ST_WALK;
	}
	if(keys[SDLK_d] && !lCheck)						/*Move right*/
	{
		if(self->vx < 10)self->vx += 5;
		if(uCheck)self->state = ST_WALK;
	}

	if(keys[SDLK_SPACE] && self->vy >= 0)			/*Jump*/
	{
		self->vy = -20;
		self->state = ST_JUMP;
		self->frame = 4;
		uCheck = 0;
	}

	switch(self->state)
	{
		case ST_DEAD:
			exit(1);
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
		case ST_JUMP:
			if(self->delay <= 0)
			{
				self->delay = 3;
				switch(self->frame)
				{
					case 4: self->frame = 5;
						break;
					case 5: self->frame = 4;
						break;
					case 6: self->frame = 7;
						break;
					case 7: self->frame = 6;
						break;
				}
			}
			else self->delay--;
			break;
	}
}

Entity *MakeTile(int x, int y, int w, int h)
{
	Entity *tile;
	tile = NewEntity();
	if(tile == NULL)return tile;
	tile->sprite = LoadSprite("images/tile.png", w, h, -1, -1, -1);
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

Entity *MakeColumn(int x, int y, int w, int h)
{
	Entity *column;
	column = NewEntity();
	if(column == NULL)return column;
	column->sprite = LoadSprite("images/small_column.png", w, h, -1, -1, -1);
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
