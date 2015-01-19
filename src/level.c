#include <stdlib.h>
#include "level.h"
#include "entity.h"
#include "hud.h"
#include "audio.h"

extern SDL_Surface *screen;
extern float offset;
extern float onset;

int CurrentLevel = 1;
int CurrentSection = 0;
int lives = 3;
int delay = 30;
float cx = 100;		/*Checkpoint position*/
float cy = 550;		/*Checkpoint position*/
float coff = 0;		/*Checkpoint offset*/
float csec = 0;		/*Checkpoint section*/
Entity *player;
Entity *boss;
Mix_Music *BgMusic = NULL;
char *bgmusic;

void RenderLevel(int level)
{
	if(level == 0)
	{
		if(CurrentSection == 0)
		{
			SpawnDrill(600, 600);
			
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);
			BuildWall(0, 600-256);
			//SpawnCloud();
			//SpawnPixie(1200);
			//SpawnFrog(900, 600-28, 0);
			//SpawnBall(900, 300);
			
			//BuildCloudPlatform(300, 350);
			//BuildCloudPlatform(425, 250);
			//BuildMovingPlatform(400, 500, 600, 500);
			//BuildMovingPlatform(200, 400, 500, 400);
			//BuildSpring(24, 600-48);
			//BuildSpring(300, 600-48);
			//BuildSpikeWall(100, 1000, 3);

			CurrentSection++;
		}
	}
	if(level == 1)
	{
		if(CurrentSection == 0)
		{
			LoadMusic("sounds/bgm_blimpoff.wav");
			BuildSmallTile(-64);
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);
			BuildColumn(400, 600-64);
			BuildColumn(800, 600-64);
			BuildColumn(800, 600-128);
			
			SpawnSnake(600, 600-32, 0);

			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildTile(1024);
			BuildTile(1408);
			BuildTile(1536);
			BuildTile(1792);

			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			SpawnSnake(2040, 600-32, 0);
			SpawnSnake(2080, 600-32, 0);
			SpawnSnake(2120, 600-32, 0);
			
			BuildTile(2048);
			BuildTile(2304);
			BuildTile(2560);
			BuildTile(2816);
			BuildPlatform(2410, 500);
			BuildColumn(3000, 600-64);
			BuildColumn(3000, 600-128);
			BuildColumn(3000, 600-192);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			BuildTile(3328);
			BuildTile(3584);
			BuildTile(3840);

			CurrentSection++;
		}
		if(CurrentSection == 8)
		{
			SpawnEye(4100, 450, 0);

			BuildFlag(4096, 600-48, 2);
			BuildTile(4096);
			BuildTile(4352);
			BuildPlatform(4480, 500);
			BuildPlatform(4736, 500);
			BuildPlatform(4992, 500);
			BuildPlatform(4864, 400);

			CurrentSection++;
		}
		if(CurrentSection == 10)
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
		if(CurrentSection == 12)
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
		if(CurrentSection == 14)
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
			BuildFlag(8200, 600-48, 1);

			SpawnSnake(7400, 600-32, 0);
			SpawnSnake(7550, 600-32, 0);
			SpawnSnake(7700, 600-32, 0);
			SpawnEye(8200, 450, 0);
			SpawnEye(8200, 450, 1);
			SpawnEye(8200, 450, 2);

			CurrentSection++;
		}
	}

	if(level == 2)
	{
		if(CurrentSection == 0)
		{			
			LoadMusic("sounds/bgm_girdergrapple.wav");
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

			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildPlatform(1152, 350);
			BuildPlatform(1664, 200);
			BuildPlatform(1536, 600);

			CurrentSection++;
		}
		if(CurrentSection == 4)
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
		if(CurrentSection == 6)
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
		if(CurrentSection == 8)
		{
			BuildFlag(4096, 600-48, 2);
			BuildTile(4096);
			BuildTile(4352);
			BuildTile(4608);
			BuildTile(4864);

			BuildSnakePot(4928, 600-64, 0, 20);

			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			BuildSmallTile(5312);
			BuildSmallTile(5600);
			BuildSmallTile(5888);
			BuildColumn(5312, 600-64);
			BuildColumn(5600, 600-64);
			BuildColumn(5600, 600-128);
			BuildColumn(5888, 600-64);
			BuildPlatform(5504, 200);
			
			BuildSnakePot(5600, 200-64, 0, 10);
			BuildSnakePot(5632, 200-64, 1, 10);

			CurrentSection++;
		}
		if(CurrentSection == 12)
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
			BuildWall(6778, 320);
			BuildWall(7010, 200);
			BuildWall(7138, 0);
			BuildWall(7138, 256);

			BuildSnakePot(6784, 200-64, 0, 60);
			BuildSnakePot(6808, 512-64, 1, 180);

			CurrentSection++;
		}
		if(CurrentSection == 14)
		{
			BuildPlatform(7296, 600);
			BuildPlatform(7680, 500);
			BuildPlatform(7232, 400);
			BuildPlatform(7744, 300);
			BuildPlatform(7168, 200);
			BuildPlatform(7808, 108);
			BuildWall(8034, 120);
			BuildWall(8034, 376);
			BuildWall(8192, 0);
			BuildWall(8192, 256);
			BuildTile(8064);
			BuildTile(8192);
			BuildTile(8448);
			BuildFlag(8200, 600-48, 1);

			SpawnEye(9500, 500, 0);
			SpawnEye(9500, 500, 1);
			SpawnEye(9500, 500, 2);
			SpawnEye(10000, 400, 0);
			SpawnEye(10000, 400, 1);
			SpawnEye(10000, 400, 2);
			SpawnEye(10000, 400, 3);
			SpawnEye(10500, 250, 0);
			SpawnEye(10500, 250, 1);
			SpawnEye(10500, 250, 2);
			SpawnEye(11000, 100, 0);
			SpawnEye(11000, 100, 1);
			SpawnEye(11000, 100, 2);
			SpawnEye(11000, 100, 3);
			SpawnEye(11500, 50, 0);
			SpawnEye(11500, 50, 1);
			SpawnEye(11500, 50, 2);
			
			CurrentSection++;
		}
	}

	if(level == 3)
	{
		if(CurrentSection == 0)
		{
			LoadMusic("sounds/bgm_bigapecity.wav");
			BuildSmallTile(-64);
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);
			BuildColumn(512, 600-64);
			BuildSpring(976, 600-48);
			BuildWall(1024, 600-256);

			SpawnFrog(650, 600-28, 0);

			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildTile(1024);
			BuildTile(1280);
			BuildTile(1536);
			BuildTile(1792);
			BuildPlatform(1024, 332);
			BuildPlatform(1280, 332);
			BuildPlatform(1280, 466);
			BuildPlatform(1536, 466);
			BuildWall(1792, 0);
			BuildWall(1792, 222);

			SpawnFrog(1500, 332-28, 0);
			SpawnFrog(1300, 466-28, 1);
			SpawnFrog(1792, 600-28, 0);
			
			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			BuildSmallTile(2048);
			BuildSpring(2064, 600-48);
			BuildCloudPlatform(2048, 350);
			BuildCloudPlatform(2064, 250);
			BuildCloudPlatform(2032, 150);
			BuildCloudPlatform(2224, 150);
			BuildCloudPlatform(2688, 350);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			BuildCloudPlatform(3072, 550);
			BuildCloudPlatform(3264, 550);
			BuildCloudPlatform(3456, 550);
			BuildCloudPlatform(3648, 550);
			BuildCloudPlatform(3200, 450);
			BuildCloudPlatform(3700, 450);
			BuildWall(3968, 600-256);
			BuildTile(3968);
			
			SpawnCloud();

			CurrentSection++;
		}
		if(CurrentSection == 8)
		{
			BuildFlag(4096, 600-48, 2);
			BuildTile(4096);
			BuildTile(4352);
			BuildSpring(4530, 600-48);
			BuildWall(4578, 408);
			BuildPlatform(4608, 408);
			BuildSpring(4816, 408-48);
			BuildWall(4864, 164);
			BuildPlatform(4894, 164);
			BuildSnakePot(4992, 100, 0, 60);
			
			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			BuildCloudPlatform(5150, 200);
			BuildCloudPlatform(5408, 400);
			BuildTile(5632);
			BuildTile(5888);
			BuildWall(5888, 0);
			BuildWall(5888, 206);
			BuildPlatform(5888, 450);
			BuildSpring(5888, 600-48);
			BuildSpring(5936, 600-48);
			BuildSpring(5984, 600-48);
			BuildSpring(6032, 600-48);
			BuildSpring(6080, 600-48);
			BuildSpring(6128, 600-48);
			BuildSpring(6176, 600-48);

			SpawnCloud();
			
			CurrentSection++;
		}
		if(CurrentSection == 12)
		{
			BuildTile(6144);
			BuildTile(6400);
			BuildTile(6656);
			BuildMovingPlatform(6272, 400, 6016, 400);
			BuildMovingPlatform(6272, 350, 6784, 350);
			BuildWall(6882, 600-256);
			BuildPlatform(6912, 344);

			SpawnEye(6272, 550, 0);
			
			CurrentSection++;
		}
		if(CurrentSection == 14)
		{
			BuildMovingPlatform(7168, 400, 7424, 400);
			BuildMovingPlatform(7680, 500, 7424, 500);
			BuildMovingPlatform(7680, 600, 7936, 600);
			BuildTile(8064);
			BuildTile(8192);
			BuildTile(8448);
			BuildFlag(8200, 600-48, 1);

			CurrentSection++;
		}
	}

	if(level == 4)
	{
		if(CurrentSection == 0)
		{
			LoadMusic("sounds/bgm_cavepart.wav");
			BuildSmallTile(-64);
			BuildTile(0);
			BuildTile(256);
			BuildTile(512);
			BuildTile(768);

			SpawnPixie(640);
			
			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildTile(1024);
			BuildTile(1280);
			BuildCloudPlatform(1344, 500);
			BuildCloudPlatform(1536, 500);
			BuildMovingPlatform(2304, 450, 1792, 450);

			SpawnPixie(1408);
			SpawnPixie(1664);
			
			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			BuildTile(2560);
			BuildTile(2816);
			BuildColumn(2752, 600-64);
			BuildColumn(2752, 600-128);
			BuildColumn(3008, 600-64);
			BuildColumn(3008, 600-128);
			BuildSpikeWall(2560, 3968, 3);

			SpawnPixie(2304);

			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			BuildSmallTile(3072);
			BuildSmallTile(3328);
			BuildSpring(3336, 600-48);
			BuildCloudPlatform(3328, 350);
			BuildCloudPlatform(3200, 275);
			BuildPlatform(3328, 200);
			BuildPlatform(3584, 200);
			BuildWall(3810, 212);
			BuildWall(3810, 408);
			BuildWall(4066, 0);
			BuildWall(4066, 256);
			BuildTile(3840);

			SpawnCloud();

			CurrentSection++;
		}
		if(CurrentSection == 8)
		{
			SpawnDrill(4608, 600);
			
			BuildFlag(4096, 600-48, 2);
			BuildTile(4096);
			BuildTile(4352);
			BuildTile(4608);
			BuildTile(4864);
			BuildColumn(4288, 600-64);
			BuildColumn(4288, 600-128);
			BuildColumn(4864, 600-64);
			BuildColumn(4864, 600-128);

			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			SpawnDrill(6016, 600);
			
			BuildSmallTile(5120);
			BuildSmallTile(5248);
			BuildSmallTile(5376);
			BuildSmallTile(5504);
			BuildSmallTile(5632);
			BuildTile(5760);
			BuildTile(6016);
			BuildColumn(5888, 600-64);

			BuildSpikes(5120, 600-32, 60, 0);
			BuildSpikes(5248, 600-32, 60, 45);
			BuildSpikes(5376, 600-32, 60, 0);
			BuildSpikes(5504, 600-32, 60, 45);
			BuildSpikes(5632, 600-32, 60, 0);
			BuildSpikes(5760, 600-32, 60, 45);
			
			CurrentSection++;
		}
		if(CurrentSection == 12)
		{
			BuildTile(6400);
			BuildTile(6656);
			BuildTile(6912);
			BuildColumn(7104, 600-64);
			BuildColumn(7104, 600-128);
			BuildSpikeWall(6656, 8140, 4);

			SpawnBall(6528, 300);
			
			CurrentSection++;
		}
		if(CurrentSection == 14)
		{
			BuildSmallTile(7296);
			BuildSmallTile(7360);
			BuildColumn(7360, 600-64);
			BuildColumn(7360, 600-128);
			BuildColumn(7360, 600-192);
			BuildColumn(7360, 600-256);
			BuildSpring(7304, 600-48);
			BuildSpring(7368, 600-304);
			BuildCloudPlatform(7360, 146);
			BuildPlatform(7552, 150);
			
			SpawnBall(7680, 50);
			BuildMovingPlatform(7808, 200, 8000, 200);
			BuildMovingPlatform(8128, 250, 7936, 250);
			BuildMovingPlatform(8128, 300, 7808, 300);
			BuildMovingPlatform(8064, 350, 7872, 350);
			BuildMovingPlatform(7936, 400, 8128, 400);
			BuildMovingPlatform(8000, 450, 7808, 450);
			BuildMovingPlatform(7808, 500, 8128, 500);
			BuildMovingPlatform(7872, 550, 8064, 550);
			
			BuildWall(8192, 0);
			BuildWall(8192, 256);
			BuildTile(8064);
			BuildTile(8192);
			BuildTile(8448);
			BuildFlag(8200, 600-48, 1);

			CurrentSection++;
		}
	}

	if(level == 5)
	{
		
		if(CurrentSection == 0)
		{
			LoadMusic("sounds/bgm_kroolacid.wav");
			BuildSmallTile(84);
			BuildSmallTile(512);
			BuildColumn(512, 600-64);
			BuildSmallTile(896);
			BuildSpring(904, 600-48);
			BuildMovingPlatform(896, 400, 1280, 400);

			SpawnCloud();
			SpawnPixie(768);
			
			CurrentSection++;
		}
		if(CurrentSection == 2)
		{
			BuildPlatform(1536, 350);
			BuildPlatform(1792, 350);
			BuildSpikes(1536, 350-31, 40, 0);
			BuildSpikes(1664, 350-31, 40, 0);
			BuildSpikes(1792, 350-31, 40, 0);
			BuildSpikes(1920, 350-31, 40, 0);

			SpawnPixie(1280);
			
			CurrentSection++;
		}
		if(CurrentSection == 4)
		{
			SpawnDrill(2256, 600);
			SpawnDrill(2640, 600);
			SpawnDrill(3024, 600);
			
			BuildSmallTile(2240);
			BuildSmallTile(2624);
			BuildSmallTile(3008);

			SpawnEye(2304, 150, 0);
			SpawnEye(2304, 150, 1);
			SpawnEye(2304, 150, 2);
			SpawnEye(2304, 150, 3);
			SpawnEye(2304, 150, 4);
			
			SpawnBall(3472, 100);
			
			CurrentSection++;
		}
		if(CurrentSection == 6)
		{
			BuildWall(3313, 208);
			BuildWall(3313, 464);
			BuildWall(3569, 0);
			BuildWall(3569, 256);
			BuildWall(3825, 208);
			BuildWall(3825, 464);

			BuildCloudPlatform(3200, 450);
			BuildCloudPlatform(3264, 350);
			BuildCloudPlatform(3232, 250);

			BuildSmallTile(3424);
			BuildSmallTile(3680);
			BuildSpikes(3424, 600-32, 60, 0);
			BuildSpikes(3680, 600-32, 60, 45);

			BuildMovingPlatform(3599, 500, 3761, 500);
			BuildMovingPlatform(3761, 450, 3599, 450);
			BuildMovingPlatform(3599, 400, 3761, 400);
			BuildMovingPlatform(3761, 350, 3599, 350);
			BuildMovingPlatform(3599, 300, 3761, 300);
			BuildMovingPlatform(3761, 250, 3599, 250);
			BuildTile(3968);
			
			CurrentSection++;
		}
		if(CurrentSection == 8)
		{
			BuildFlag(4096, 600-48, 2);
			BuildTile(4096);
			BuildTile(4224);
			BuildTile(4608);
			BuildTile(4992);

			BuildSnakePot(4576, -256, 0, 60);
			BuildSnakePot(4704, -128, 0, 50);
			BuildSnakePot(4832, -192, 0, 55);
			BuildSnakePot(4960, -64, 0, 45);
			BuildSnakePot(5088, -128, 0, 50);
			
			CurrentSection++;
		}
		if(CurrentSection == 10)
		{
			BuildSmallTile(5568);
			BuildSmallTile(5952);
			BuildSpring(5200, 600-48);
			BuildSpring(5576, 600-48);
			BuildSpring(5960, 600-48);

			SpawnPixie(5504);
			SpawnPixie(5632);
			SpawnPixie(5760);
			SpawnPixie(5888);
			SpawnPixie(6016);
			SpawnPixie(6144);
			SpawnPixie(6272);
			SpawnPixie(6400);
			
			CurrentSection++;
		}
		if(CurrentSection == 12)
		{
			BuildPlatform(6400, 700);
			BuildPlatform(6656, 700);
			BuildPlatform(6912, 700);
			BuildPlatform(6400, 712);
			BuildPlatform(6656, 712);
			BuildPlatform(6912, 712);
			BuildPlatform(7040, 450);
			BuildPlatform(7080, 250);
			BuildCloudPlatform(6656, 200);
			BuildCloudPlatform(6976, 200);
			BuildSpikeWall(6144, 8140, 2);

			BuildColumn(6592, 700-64);
			BuildColumn(6784, 700-64);
			BuildColumn(6976, 700-64);
			BuildSpring(7120, 700-48);
			BuildSpring(7040, 450-48);

			SpawnFrog(6692, 700-28, 0);
			SpawnFrog(6912, 700-28, 0);
			SpawnFrog(7060, 700-28, 0);
			SpawnFrog(6800, 200-24, 0);
			SpawnFrog(7100, 200-24, 0);

			CurrentSection++;
		}
		if(CurrentSection == 14)
		{
			BuildWall(7424, 208);
			BuildWall(7424, 464);
			BuildSmallTile(7360);
			BuildSmallTile(7616);
			BuildSmallTile(7808);
			BuildSmallTile(8000);
			BuildTile(8064);
			BuildTile(8192);
			BuildTile(8448);
			BuildFlag(8200, 600-48, 1);
			
			BuildCloudPlatform(7296, 200);
			BuildSpring(7368, 600-48);
			BuildSpikes(7616, 600-32, 40, 0);
			BuildSpikes(7808, 600-32, 40, 0);
			BuildSpikes(8000, 600-32, 40, 0);

			SpawnEye(8832, 100, 0);
			SpawnEye(8832, 100, 1);
			SpawnEye(8832, 100, 2);
			SpawnEye(8832, 200, 3);
			SpawnEye(8832, 200, 4);
			SpawnEye(8832, 200, 5);
			SpawnEye(8832, 300, 6);
			SpawnEye(8832, 300, 7);
			SpawnEye(8832, 300, 8);
			SpawnEye(8832, 400, 9);
			SpawnEye(8832, 400, 10);
			SpawnEye(8832, 400, 11);

			CurrentSection++;
		}
	}

	if(level == 6)
	{
		if(CurrentSection == 0)
		{
			LoadMusic("sounds/bgm_flameheart.wav");
			BuildTile(96);
			BuildTile(352);
			BuildTile(608);

			boss = SpawnBoss();

			CurrentSection++;
		}
		if(CurrentSection > 0 && boss->sy >= 800)
		{
			CurrentLevel += ContinueScreen();
		}
	}
}

void UpdateLevel()
{
	if(!lives)
	{
		if(ContinueScreen())	/*Continue*/
		{
			cx = 100;		/*Reset checkpoint stuff*/
			cy = 550;
			coff = 0;
			csec = 0;
			lives = 3;
		}
		else exit(1);			/*Game Over*/
	}	
	
	if(player == NULL)	/*Make a new player at the last checkpoint*/
	{
		player = MakePlayer(cx, cy);
		offset = coff;
		onset = coff;
		CurrentSection = csec;
	}
	
	/*Scroll Screen with Player*/
	if(CurrentLevel < 6)
	{
		if(player->sx >= (screen->w * 0.5) + offset)offset += player->sx - ((screen->w * 0.5) + offset);						/*Right*/
		if(player->sx <= (screen->w * 0.25) + offset && offset > onset)offset -= ((screen->w * 0.25) + offset) - player->sx;		/*Left*/
		if(onset + 1024 < offset)onset = offset - 1024;
	}

	/*When the player reaches a certain point, load the next section*/
	if(offset + screen->w >= 1024 && CurrentSection == 1)CurrentSection++;
	if(offset + screen->w >= 2048 && CurrentSection == 3)CurrentSection++;
	if(offset + screen->w >= 3072 && CurrentSection == 5)CurrentSection++;
	if(offset + screen->w >= 4096 && CurrentSection == 7)CurrentSection++;
	if(offset + screen->w >= 5120 && CurrentSection == 9)CurrentSection++;
	if(offset + screen->w >= 6144 && CurrentSection == 11)CurrentSection++;
	if(offset + screen->w >= 7168 && CurrentSection == 13)CurrentSection++;

	if(player->sx >= 4096)		/*Sets checkpoint*/
	{
		cx = 4196;
		cy = 550;
		coff = 4096;
		csec = 8;
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
		if(delay <= 0)
		{
			lives--;
			player = NULL;
			ClearEntities();
			delay = 30;
		}
		else delay--;
		return;
	}

	if(CurrentLevel == 6 && CurrentSection == 1 && boss->sy > 800)CurrentLevel += WinScreen();
	
	if(CurrentLevel == 7)exit(1);	/*Win Condition*/
	
	RenderLevel(CurrentLevel);
}

void LoadMusic(char *bg)
{
	if(BgMusic != NULL)
    {
      Mix_HaltMusic();
      Mix_FreeMusic(BgMusic);
    }
	BgMusic = Mix_LoadMUS(bg);
	if(BgMusic == NULL)
	{
		fprintf(stderr, "Unable to Load Background Music: %s\n", SDL_GetError());
		exit(1);
	}
	Mix_PlayMusic(BgMusic,-1);	/*play my background music infinitely*/
}
