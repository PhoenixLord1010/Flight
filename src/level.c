#include "level.h"
#include "entity.h"

int CurrentLevel = 0;
int NumLevels = 2;

void RenderLevel(int level)
{
	if(level == 0)
	{
		BuildTile(0, 600, 320, 32);
		BuildTile(320, 600, 320, 32);
		BuildTile(640, 600, 320, 32);
		BuildTile(960, 600, 320, 32);
		BuildColumn(400, 600-64, 64, 64, 1);
		BuildColumn(800, 600-128, 64, 128, 2);
		SpawnSnake(600, 600-32);
		SpawnSnake(710, 600-32);
		SpawnSnake(820, 600-160);
	}
}

void UpdateLevel()
{
}
