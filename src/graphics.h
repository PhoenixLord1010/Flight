#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

/*color constants:*/
enum colors {Red = 1,Green = 2,Blue = 3,Yellow = 4,Orange = 5,Violet = 6,Brown = 7,Grey = 8,DarkRed = 9,DarkGreen = 10,
               DarkBlue = 11,DarkYellow = 12,DarkOrange = 13,DarkViolet = 14,DarkBrown = 15,DarkGrey = 16,LightRed =17,
               LightGreen = 18,LightBlue = 19,LightYellow = 20,LightOrange = 21,LightViolet = 22,LightBrown = 23,LightGrey = 24,
               Black = 25,White = 26,Tan = 27,Gold = 28,Silver = 29,YellowGreen = 30,Cyan = 31,Magenta = 32};

typedef struct Sprite_T
{
	SDL_Surface *image;
	char filename[20];
	int w,h;
	int framesperline;
	int color1,color2,color3;
	int used;
}SPRITE;

typedef struct
{
	int xres,yres,depth;
}ScreenData;

//Basics
void Init_Graphics();
void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y);
void DrawLine(SDL_Surface *screen,Uint32 color,int sx,int sy,int gx,int gy);
void ShowImage(SDL_Surface *image, SDL_Surface *screen, int x, int y);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void DrawFilledRect(int sx,int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface);
void DrawRect(int sx,int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface);

//Sprite Functions
void InitSpriteList();
void FreeSprite(SPRITE *img);
SPRITE *LoadSprite(char *filename, int sizex, int sizey, int c1, int c2, int c3);
void DrawSprite(SPRITE *sprite, SDL_Surface *surface, int sx, int sy, int frame);
void DrawSpritePixel(SPRITE *sprite, SDL_Surface *surface, int sx, int sy, int frame);
void CloseSprites();

//Palette Swapping Functions
SPRITE *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3);
Uint32 SetColor(Uint32 color, int newcolor1,int newcolor2, int newcolor3);
void SwapSprite(SDL_Surface *sprite,int color1,int color2,int color3);
Uint32 IndexColor(int color);

//Frame Handling Functions
void BlankScreen(SDL_Surface *buf, Uint32 color);
void FrameDelay(Uint32 delay);
void ResetBuffer();
void NextFrame();

/*
void ShowBMP(SDL_Surface *image, SDL_Surface *screen, int x, int y);
void DrawWindow(SPRITE *sprite, SDL_Surface *surface);
void PaintWindow(SDL_Surface *buffer, int x, int y, int w, int h, Uint32 color);
SPRITE *NewWindow(int sx, int sy, int sw, int sh, int bgcolor);
SPRITE *DisplayBar();
void InitMouse();
void DrawMouse();
*/

#endif