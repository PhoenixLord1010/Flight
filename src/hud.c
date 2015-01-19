#include "hud.h"
#include "level.h"
#include "audio.h"

enum Button_States {B_Normal,B_Press,B_Highlight};

extern Entity *Player;
extern Entity *Boss;
extern SDL_Surface *screen;
extern int lives;
extern int CurrentLevel;
int keymoved = 0;


/* Window Maintenance */
void DrawStatusBarHoriz(int stat,int range,int FG,int BG,int x, int y, int w, int h)
{
  float percent;
  DrawFilledRect(x, y, w, h, IndexColor(BG), screen);
  if((stat > 0) && (range != 0))
  {
    percent = ((stat-0.4) * w) / (range-0.4);
    DrawFilledRect(x, y, percent, h, IndexColor(FG), screen);
  }

}

/* HUD Maintenance */
void DrawHUD()
{
	if(Player != NULL)
	{
		int i;
		char *num;
		
		/*Player stuff*/
		DrawStatusBarHoriz(Player->health, Player->healthmax, Green, Red, 10, 10, 160, 13);
		DrawText("Health", screen, 11, 9, IndexColor(White), F_Small);
		DrawText("Lives:", screen, 200, 9, IndexColor(White), F_Small);
		for(i=0; i<lives; i++)
		{
			DrawText("*", screen, 250 + (7 * i), 9, IndexColor(White), F_Small);
		}

		/*Boss stuff*/
		if(CurrentLevel == 6 && Boss != NULL)
		{
			DrawStatusBarHoriz(Boss->health, Boss->healthmax, DarkGreen, DarkRed, 790, 10, 160, 13);
			DrawText("Boss Health", screen, 791, 9, IndexColor(White), F_Small);
		}

		switch(CurrentLevel)
		{
			case 0:
				num = "Test";
				break;
			case 1:
				num = "Level 1";
				break;
			case 2:
				num = "Level 2";
				break;
			case 3:
				num = "Level 3";
				break;
			case 4:
				num = "Level 4";
				break;
			case 5:
				num = "Level 5";
				break;
			case 6:
				num = "Boss Level";
				break;
		}

		/*Level stuff*/
		DrawText(num, screen, 450, 9, IndexColor(White), F_Small);
  }
}

int ContinueScreen()
{
	int done = 0;
	int cont = 1;
	int keyn;
	Uint8 *keys;
	
	LoadMusic("sounds/bgm_continue.wav");
	do
	{
		ResetBuffer();
		SDL_PumpEvents();
		
		if(isKeyPressed(SDLK_RETURN))done = 1;
		if(isKeyPressed(SDLK_t))cont = 0;
		if(isKeyPressed(SDLK_e))cont = 1;
		
		DrawText("<Continue?>", screen, screen->w/2 - 180, screen->h/2.5, IndexColor(White), F_Large);
		DrawText("Yes", screen, screen->w/2 - 3, screen->h/1.8, IndexColor(White), F_Small);
		DrawText("No", screen, screen->w/2 - 3, screen->h/1.7, IndexColor(White), F_Small);

		if(cont)DrawText(">", screen, screen->w/2 - 13, screen->h/1.8, IndexColor(White), F_Small);
		else DrawText(">", screen, screen->w/2 - 13, screen->h/1.7, IndexColor(White), F_Small);

		NextFrame();
	}while(!done);

	return cont;
}

int WinScreen()
{
	int done = 150;
	
	LoadMusic("sounds/bgm_bossvictory.wav");
	do
	{
		ResetBuffer();
		SDL_PumpEvents();

		DrawText("Hey, you did it", screen, screen->w/2 - 60, screen->h/2.5, IndexColor(White), F_Small);

		done--;

		NextFrame();
	}while(done);

	return 1;
}
