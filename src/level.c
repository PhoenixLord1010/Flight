#include "level.h"
#include "entity.h"

int CurrentLevel = 0;
int NumLevels = 2;

void RenderLevel(int level)
{
	if(level == 0)
	{
		MakeTile(0, 600, 320, 32);
		MakeTile(320, 600, 320, 32);
		MakeTile(640, 600, 320, 32);
		MakeTile(960, 600, 320, 32);
		MakeColumn(400, 600-64, 64, 64);
		MakeColumn(800, 600-64, 64, 64);
		SpawnSnake(600, 600-32);
	}
}

void UpdateLevel()
{
}
