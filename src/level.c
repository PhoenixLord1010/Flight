#include "level.h"
#include "entity.h"

extern SDL_Surface *screen;
extern float offset;

int CurrentLevel = 0;
int CurrentSection = 0;
int lives = 3;
int delay = 30;
float cx = 100;		/*Checkpoint position*/
float cy = 550;		/*Checkpoint position*/
float coff = 0;		/*Checkpoint offset*/
float csec = 0;		/*Chackpoint section*/
Entity *player;

void RenderLevel(int level)
{
	if(level == 0)
	{
		if(CurrentSection == 0)
		{
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);
			BuildColumn(400, 600-64);
			BuildColumn(800, 600-64);
			BuildColumn(800, 600-128);
			
			SpawnSnake(600, 600-32, 0);

			BuildTile(1024);
			BuildTile(1408);
			BuildTile(1536);
			BuildTile(1792);
			
			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			SpawnSnake(2040, 600-32, 0);
			SpawnSnake(2080, 600-32, 0);
			SpawnSnake(2120, 600-32, 0);
			
			BuildTile(2048);
			BuildTile(2304);
			BuildTile(2560);
			BuildTile(2816);
			BuildPlatform(2310, 500);
			BuildColumn(3000, 600-64);
			BuildColumn(3000, 600-128);
			BuildColumn(3000, 600-192);

			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			BuildTile(3328);
			BuildTile(3584);
			BuildTile(3840);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			SpawnEye(4100, 450, 0);

			BuildTile(4096);
			BuildTile(4352);
			BuildPlatform(4480, 500);
			BuildPlatform(4736, 500);
			BuildPlatform(4992, 500);
			BuildPlatform(4864, 400);

			CurrentSection++;
		}
		if(CurrentSection == 8)
		{	
			BuildPlatform(5120, 400);
			BuildPlatform(5248, 500);
			BuildTile(5888);

			SpawnSnake(5120, 400-32, 0);
			SpawnSnake(5160, 400-32, 0);
			SpawnSnake(5200, 400-32, 0);

			SpawnEye(6144, 450, 0);
			SpawnEye(6144, 450, 1);

			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			BuildTile(6144);
			BuildTile(6400);
			BuildTile(6656);
			BuildTile(6912);
			BuildPlatform(6300, 200);
			BuildPlatform(6684, 200);

			SpawnSnake(6428, 200-32, 0);
			SpawnSnake(6930, 200-32, 0);

			CurrentSection++;
		}
		if(CurrentSection == 12)
		{
			BuildTile(7168);
			BuildTile(7424);
			BuildTile(7680);
			BuildTile(7936);
			BuildTile(8192);
			BuildTile(8448);
			BuildColumn(7232, 600-64);
			BuildColumn(7232, 600-128);
			BuildColumn(7808, 600-64);
			BuildColumn(7808, 600-128);

			SpawnSnake(7400, 600-32, 0);
			SpawnSnake(7550, 600-32, 0);
			SpawnSnake(7700, 600-32, 0);
			SpawnEye(8200, 450, 0);
			SpawnEye(8200, 450, 1);
			SpawnEye(8200, 450, 2);

			CurrentSection++;
		}
	}

	if(level == 1)
	{
		if(CurrentSection == 0)
		{			
			BuildSmallTile(-64);
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);
			BuildColumn(448, 600-64);
			BuildColumn(704, 600-64);
			BuildColumn(704, 600-128);
			BuildColumn(960, 600-64);
			BuildColumn(960, 600-128);
			BuildColumn(960, 600-192);

			BuildPlatform(1152, 350);
			BuildPlatform(1664, 200);
			BuildPlatform(1536, 600);

			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildTile(2048);
			BuildTile(2304);
			BuildTile(2560);
			BuildTile(2816);
			BuildPlatform(2176, 200);
			BuildPlatform(2432, 200);
			BuildPlatform(2688, 200);
			BuildPlatform(2944, 200);

			SpawnSnake(2304, 200-32, 0);
			SpawnSnake(2560, 600-32, 0);
			SpawnSnake(2688, 600-32, 0);
			SpawnSnake(2816, 600-32, 0);
			SpawnSnake(2944, 600-32, 0);
			SpawnEye(3200, 550, 0);
			SpawnEye(3200, 550, 1);
			SpawnEye(3200, 550, 2);
			SpawnEye(3000, 120, 0);

			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			BuildTile(3200);
			BuildSmallTile(3712);
			BuildTile(3968);
			BuildColumn(3392, 600-64);
			BuildColumn(3392, 600-128);
			BuildColumn(3712, 600-64);
			BuildColumn(3968, 600-64);
			BuildColumn(3968, 600-128);
			BuildWall(3584, 0);
			BuildPlatform(3712, 260);

			SpawnEye(4096, 400, 0);
			SpawnEye(4096, 400, 1);
			SpawnEye(4096, 400, 2);
			SpawnEye(4608, 400, 0);
			SpawnEye(4608, 400, 1);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			BuildTile(4096);
			BuildTile(4352);
			BuildTile(4608);
			BuildTile(4864);

			BuildSnakePot(4928, 600-64, 0, 20);

			CurrentSection++;
		}
		if(CurrentSection == 8)
		{
			BuildSmallTile(5312);
			BuildSmallTile(5600);
			BuildSmallTile(5888);
			BuildColumn(5312, 600-64);
			BuildColumn(5600, 600-64);
			BuildColumn(5600, 600-128);
			BuildColumn(5888, 600-64);
			BuildPlatform(5248, 150);
			BuildPlatform(5504, 200);
			BuildPlatform(5760, 150);
			
			BuildSnakePot(5248, 150-64, 1, 15);
			BuildSnakePot(5984, 150-64, 0, 15);

			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			BuildTile(6144);
			BuildTile(6400);
			BuildTile(6912);
			BuildSmallTile(7168);
			BuildPlatform(6144, 500);
			BuildPlatform(6272, 500);
			BuildPlatform(6272, 400);
			BuildPlatform(6400, 400);
			BuildPlatform(6144, 300);
			BuildPlatform(6272, 300);
			BuildPlatform(6272, 200);
			BuildPlatform(6528, 200);
			BuildPlatform(6784, 200);
			BuildPlatform(6784, 512);
			BuildPlatform(6912, 512);
			BuildWall(6144, 0);
			BuildWall(6144, 256);
			BuildWall(6656, 212);
			BuildWall(6656, 376);
			BuildWall(6784, 320);
			BuildWall(7016, 200);
			BuildWall(7144, 0);
			BuildWall(7144, 256);

			BuildSnakePot(6784, 200-64, 0, 60);
			BuildSnakePot(6808, 512-64, 1, 180);

			CurrentSection++;
		}
		if(CurrentSection == 12)
		{
			BuildPlatform(7424, 600);
			BuildPlatform(7680, 500);
			BuildPlatform(7360, 400);
			BuildPlatform(7744, 300);
			BuildPlatform(7296, 200);
			BuildPlatform(7808, 100);
			BuildWall(8040, 112);
			BuildWall(8040, 368);
			BuildWall(8192, 0);
			BuildWall(8192, 256);
			BuildTile(8064);
			BuildTile(8192);
			BuildTile(8448);
		}
	}
}

void UpdateLevel()
{
	if(!lives)exit(1);	/*Game Over*/
	
	if(player == NULL)	/*Make a new player at the last checkpoint*/
	{
		player = MakePlayer(cx, cy);
		offset = coff;
		CurrentSection = csec;
	}

	if(player->sx >= (screen->w * 0.6) + offset)offset += player->sx - ((screen->w * 0.6) + offset);	/*Scroll Screen with Player*/

	/*When the player reaches a certain point, load the next section*/
	if(offset + screen->w >= 2028 && CurrentSection == 1)CurrentSection++;
	if(offset + screen->w >= 3052 && CurrentSection == 3)CurrentSection++;
	if(offset + screen->w >= 4076 && CurrentSection == 5)CurrentSection++;
	if(offset + screen->w >= 5100 && CurrentSection == 7)CurrentSection++;
	if(offset + screen->w >= 6124 && CurrentSection == 9)CurrentSection++;
	if(offset + screen->w >= 7148 && CurrentSection == 11)CurrentSection++;

	if(player->sx >= 4096)		/*Sets checkpoint*/
	{
		cx = 4196;
		cy = 550;
		coff = 4096;
		csec = 6;
	}

	/*End of Level*/
	if(player->sx >= 8200)
	{
		player->vx = 0;			/*Freezes the player, kinda*/
		player->vy = 0;
		player->invuln = 30;
		if(delay <= 0)
		{
			player = NULL;
			ClearEntities();
			cx = 100;			/*Reset checkpoint stuff*/
			cy = 550;
			coff = 0;
			csec = 0;
			CurrentLevel++;
			delay = 30;
			return;
		}
		else delay--;
	}

	if(player->state == ST_DEAD)	/*Player died*/
	{	
		lives--;
		player = NULL;
		ClearEntities();
		return;
	}
	
	if(CurrentLevel == 2)exit(1);	/*Win Condition*/

	RenderLevel(CurrentLevel);
}
