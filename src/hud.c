#include "hud.h"

enum Button_States {B_Normal,B_Press,B_Highlight};

extern Entity *Player;
extern SDL_Surface *screen;
int keymoved = 0;


Mouse_T Mouse;
Sprite *buttonsprites[3];
Sprite *buttonarrows[3];
Sprite *windowsprite;
Sprite *mediumwindowsprite;
Sprite *elements;
Sprite *attributes;

/* Window Maintenance */
void DrawStatusBarHoriz(int stat,int range,int FG,int BG,int x, int y, int w, int h)
{
  float percent;
  DrawFilledRect(x, y, w, h, IndexColor(BG), screen);
  if((stat > 0) && (range != 0))
  {
    percent = (stat * w) / range;
    DrawFilledRect(x, y, percent, h, IndexColor(FG), screen);
  }

}

/* HUD Maintenance */
void DrawHUD()
{
  if(Player != NULL)
	  DrawStatusBarHoriz(Player->health, Player->healthmax, Green, Red, 10, 10, 160, 10);
}

/* Mouse Maintenance functions */
void InitMouse()
{
  Mouse.sprite=LoadSprite("images/mouse.png",16,16);
  Mouse.state = 0;
  Mouse.shown = 0;
  Mouse.frame = 0;
}

void DrawMouse()
{
  int mx,my;
  Mouse.oldbuttons = Mouse.buttons;
  Mouse.buttons = SDL_GetMouseState(&mx,&my);
  if(Mouse.sprite != NULL)DrawSprite(Mouse.sprite,screen,mx,my,Mouse.frame);
  Mouse.frame = ((Mouse.frame + 1)%16)+ (16 * Mouse.state);
  Mouse.ox = Mouse.x;
  Mouse.oy = Mouse.y;
  Mouse.x = mx;
  Mouse.y = my;
}

void KillMouse()
{
  FreeSprite(Mouse.sprite);
  Mouse.sprite = NULL;
}

Uint32 MouseIn(SDL_Rect rect)
{
  if((Mouse.buttons != 0)&&(Mouse.oldbuttons == 0))
  {
    if((Mouse.x >= rect.x)&&(Mouse.x < rect.x + rect.w)&&(Mouse.y >= rect.y)&&(Mouse.y < rect.y + rect.h))
      return Mouse.buttons;
  }
  return 0;
}

int MouseOver(SDL_Rect rect)
{
  if((Mouse.x >= rect.x)&&(Mouse.x < rect.x + rect.w)&&(Mouse.y >= rect.y)&&(Mouse.y < rect.y + rect.h))
    return 1;
  return 0;
}
