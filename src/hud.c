#include "hud.h"
#include "level.h"

enum Button_States {B_Normal,B_Press,B_Highlight};

extern Entity *Player;
extern SDL_Surface *screen;
extern int lives;
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
		
		DrawStatusBarHoriz(Player->health, Player->healthmax, Green, Red, 10, 10, 160, 13);
		DrawText("Health", screen, 11, 9, IndexColor(White), F_Small);
		DrawText("Lives:", screen, 200, 9, IndexColor(White), F_Small);
		for(i=0; i<lives; i++)
		{
			DrawText("*", screen, 250 + (7 * i), 9, IndexColor(White), F_Small);
		}
  }
}