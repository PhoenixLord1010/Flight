#include "level.h"
#include "entity.h"

extern SDL_Surface *screen;

int CurrentLevel = 0;
int NumLevels = 2;
int CurrentSection = 0;
int delay = 30;

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
			
			SpawnSnake(600, 600-32);

			BuildTile(1024);
			BuildTile(1408);
			BuildTile(1536);
			BuildTile(1792);
			
			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			SpawnSnake(2000, 600-32);
			SpawnSnake(2040, 600-32);
			SpawnSnake(2080, 600-32);
			
			BuildTile(2048);
			BuildTile(2304);
			BuildTile(2560);
			BuildTile(2816);
			BuildPlatform(2300, 500);
			BuildPlatform(2428, 500);
			BuildColumn(3000, 600-64);
			BuildColumn(3000, 600-128);
			BuildColumn(3000, 600-192);

			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			BuildTile(3200);
			BuildTile(3328);
			BuildTile(3584);
			BuildTile(3840);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			SpawnEye(4100, 500);

			CurrentSection++;
		}
	}
}

void UpdateLevel(int level)
{
	if(screen->offset >= 724 && CurrentSection == 1)CurrentSection++;
	if(screen->offset >= 1748 && CurrentSection == 3)CurrentSection++;
	if(screen->offset >= 2772 && CurrentSection == 5)CurrentSection++;
	RenderLevel(CurrentLevel);
}
