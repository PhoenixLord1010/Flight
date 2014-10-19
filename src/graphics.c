#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "entity.h"
#include "graphics.h"

#define MaxSprites		511
#define MaxWindows		64

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *bgimage;
//SDL_Surface *buffer;
SDL_Surface *videobuffer;
TTF_Font *font;
SDL_Rect Camera;
Sprite SpriteList[MaxSprites];
Sprite WindowList[MaxWindows];
Entity *Mouse;
Uint32 NOW;

int NumSprites;
//int NumWindows;
//extern int NumLives;
//extern Entity EntityList[MAXENTITIES];

Uint32 rmask,gmask,bmask,amask;
ScreenData S_Data;


void Init_Graphics(int windowed)
{
	Uint32 Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT;
	Uint32 HWflag = 0;
	SDL_Surface *temp;
	S_Data.xres = 1280; //1600;
	S_Data.yres = 720; //900;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
	#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
	#endif
	if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	if(SDL_VideoModeOK(1280, 720, 32, SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE))
	{
		S_Data.xres = 1280;
		S_Data.yres = 720;
		S_Data.depth = 32;
		Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE;
		HWflag = SDL_HWSURFACE;
	}
	else if(SDL_VideoModeOK(1280, 720, 16, SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE))
		 {
			 S_Data.xres = 1280;
			 S_Data.yres = 720;
			 S_Data.depth = 16;
			 Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT | SDL_HWSURFACE;
			 HWflag = SDL_HWSURFACE;
		 }
		 else if(SDL_VideoModeOK(1280, 720, 16, SDL_FULLSCREEN | SDL_ANYFORMAT))
			  {
				  S_Data.xres = 1280;
				  S_Data.yres = 720;
				  S_Data.depth = 16;
				  Vflags = SDL_FULLSCREEN | SDL_ANYFORMAT;
				  HWflag = SDL_SWSURFACE;
			  }
	videobuffer = SDL_SetVideoMode(S_Data.xres, S_Data.yres, S_Data.depth, Vflags);
	if(videobuffer == NULL)
	{
		fprintf(stderr, "Unable to set 1024x600 video: %s\n", SDL_GetError());
		exit(1);
	}
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, S_Data.xres, S_Data.yres, S_Data.depth, rmask, gmask, bmask, amask);
	if(temp == NULL)
	{
		fprintf(stderr, "Couldn't initialise background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	screen = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	temp = SDL_CreateRGBSurface(Vflags, 2560, 720, S_Data.depth, rmask, gmask, bmask, amask);
	if(temp == NULL)
	{
		fprintf(stderr, "Couldn't initialise background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	background = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(SDL_DISABLE);
	Camera.x = 0;
	Camera.y = 0;
	Camera.w = screen->w;
	Camera.h = screen->h;
	srand(SDL_GetTicks());
	
	if(TTF_Init() == 0)
		atexit(TTF_Quit);
	else
	{
		fprintf(stderr, "Couldn't initialise True Type Font: %s\n", SDL_GetError());
		exit(1);
	}
	font = TTF_OpenFont("fonts/font1.ttf",14);
	if(font == NULL)
	{
		fprintf(stderr, "Couldn't load font: %s\n", SDL_GetError());
		exit(1);
	}
}

void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y)
{
	Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    //if (SDL_MUSTLOCK(screen))
    //{
        if (SDL_LockSurface(screen) < 0)
        {
            return;
        }
    //}
    switch (screen->format->BytesPerPixel)
    {
        case 1:
        { /* Assuming 8-bpp */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
        break;

        case 2:
        { /* Probably 15-bpp or 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3:
        { /* Slow 24-bpp mode, usually not used */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

        case 4:
        { /* Probably 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
    //if (SDL_MUSTLOCK(screen))
    //{
        SDL_UnlockSurface(screen);
    //}
    SDL_UpdateRect(screen, x, y, 1, 1);
}

void DrawSquareLine(SDL_Surface *screen, Uint32 color, int sx, int sy, int gx, int gy)
{
	SDL_Rect box;
	if(sx < gx)box.x = sx;
	else box.x = gx;
	if(sy < gy)box.y = sy;
	else box.y = gy;
	if(sy == gy)
	{
		box.w = fabs(float(sx - gx));
		box.h = 1;                                        
		SDL_FillRect(screen,&box,color);    
		return;
	}
	box.h = fabs(float(sy - gy));
	box.w = 1;                                        
	SDL_FillRect(screen,&box,color);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    /* Here p is the address to the pixel we want to retrieve*/
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
	if((x < 0)||(x >= surface->w)||(y < 0)||(y >= surface->h))return -1;
    switch(surface->format->BytesPerPixel)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /*shouldn't happen, but avoids warnings*/
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	SDL_Rect point = {0,0,1,1};
  point.x = x;
  point.y = y;
  SDL_FillRect(surface,&point,pixel);
}

void DrawFilledRect(int sx, int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface)
{
	SDL_Rect dst;
	dst.x = sx;
	dst.y = sy;
	dst.w = sw;
	dst.h = sh;
	SDL_FillRect(surface, &dst, Color);
}

void DrawRect(int sx, int sy, int sw, int sh, Uint32 Color, SDL_Surface *surface)
{
	SDL_Rect box;
    box.x = sx;
    box.y = sy;
    box.w = sw;
    box.h = 1;                                        
    SDL_FillRect(surface,&box,Color);
    box.y = sy + sh;
    SDL_FillRect(surface,&box,Color);
    box.y = sy;
    box.w = 1;
    box.h = sh;
    SDL_FillRect(surface,&box,Color);
    box.x = sx + sw;
    SDL_FillRect(surface,&box,Color);
}


void InitSpriteList()
{
	int x;
	NumSprites = 0;
	//NumWindows = 0;
	memset(SpriteList, 0, sizeof(Sprite) * MaxSprites);
	for(x = 0; x < MaxSprites; x++)SpriteList[x].image = NULL;
	//memset(WindowList, 0, sizeof(Sprite) * MaxWindows);
	//for(x = 0; x < MaxWindows; x++)WindowList[x].image = NULL;
}

void FreeSprite(Sprite *sprite)
{
	sprite->used--;
	if(sprite->used <= 0)
	{
		NumSprites--;
		strcpy(sprite->filename,"\0");
		if(sprite->image != NULL)SDL_FreeSurface(sprite->image);
		sprite->image = NULL;
	}
}

Sprite *LoadSprite(char *filename, int sizex, int sizey)
{
	return LoadSwappedSprite(filename, sizex, sizey, -1, -1, -1);
}

void DrawSprite(Sprite *sprite, SDL_Surface *surface, int sx, int sy, int frame)
{
	SDL_Rect src,dest;
	if((sprite == NULL)||(surface == NULL))return;
    if((sprite->numframes != -1)&&(frame > sprite->numframes))return; /*lets not draw past our sprite*/   
	src.x = frame%sprite->framesperline * sprite->w;
    src.y = frame/sprite->framesperline * sprite->h;
    src.w = sprite->w;
    src.h = sprite->h;
	dest.x = sx - screen->offset;
    dest.y = sy;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

void DrawSpritePixel(Sprite *sprite, SDL_Surface *surface, int sx, int sy, int frame)
{
    SDL_Rect src,dest;
    src.x = frame%sprite->framesperline * sprite->w + sprite->w/2;
    src.y = frame/sprite->framesperline * sprite->h + sprite->h/2;
    src.w = 1;
    src.h = 1;
    dest.x = sx;
    dest.y = sy;
    dest.w = 1;
    dest.h = 1;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

void CloseSprites()
{
	int i;
	for(i = 0; i < MaxSprites; i++)
	{
		FreeSprite(&SpriteList[i]);
	}
	//for(i = 0; i < MaxWindows; i++)
	//{
	//	FreeSprite(&WindowList[i]);
	//}
}


Sprite *LoadSwappedSprite(char *filename, int sizex, int sizey, int c1, int c2, int c3)
{
  int i;
  SDL_Surface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if((strncmp(filename,SpriteList[i].filename,20)==0)&&(SpriteList[i].used >= 1)&&(c1 == SpriteList[i].color1)&&(c2 == SpriteList[i].color2)&&(c3 == SpriteList[i].color3))
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
        fprintf(stderr, "FAILED TO LOAD A VITAL SPRITE.\n",filename);
        return NULL;
  }
  SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 255,255,255));
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  //SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGB(SpriteList[i].image->format, 255,255,255));
  //fprintf(stderr,"asked for colors: %d,%d,%d \n",c1,c2,c3);
  SwapSprite(SpriteList[i].image,c1,c2,c3);
   /*then copy the given information to the sprite*/
  strncpy(SpriteList[i].filename,filename,80);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 16;
  if((sizex) && (sizey))/*as long as neither x and y are zero*/
  {
    SpriteList[i].numframes = (SpriteList[i].image->w / sizex) * (SpriteList[i].image->h / sizey);
  }
  else SpriteList[i].numframes = -1;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
  SpriteList[i].color1 = c1;
  SpriteList[i].color2 = c2;
  SpriteList[i].color3 = c3;
  SpriteList[i].used++;
  return &SpriteList[i];
}

Uint32 SetColor(Uint32 color, int newcolor1, int newcolor2, int newcolor3)
{
    Uint8 r,g,b;
    Uint8 intensity;
    int newcolor;
    SDL_GetRGB(color, screen->format, &r, &g, &b);
    if((r == 0) && (g == 0) && (b != 0))
    {
        intensity = b;
        newcolor = newcolor3;
    }
    else if((r == 0) && (b == 0) && (g != 0))
		 {
			 intensity = g;
			 newcolor = newcolor2;
		 }
		 else if((g == 0) && (b == 0) && (r != 0))
			  {
				  intensity = r;
				  newcolor = newcolor1;
			  }
			  else return color;
    switch(newcolor)
    {
        case Red:
            r = intensity;
            g = (Uint8)(intensity * 0.2);
            b = (Uint8)(intensity * 0.2);
            break;
        case Green:
            r = (Uint8)(intensity * 0.2);
            g = intensity;
            b = (Uint8)(intensity * 0.2);
            break;
        case Blue:
            r = (Uint8)(intensity * 0.2);
            g = (Uint8)(intensity * 0.2);
            b = intensity;
            break;
        case Yellow:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.8);
            b = (Uint8)(intensity * 0.2);
            break;
        case Orange:
            r = intensity;
            g = (Uint8)(intensity * 0.5);
            b = (Uint8)(intensity * 0.2);
            break;
        case Violet:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.2);
            b = (Uint8)(intensity * 0.8);
            break;
        case Brown:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.25);
            break;
        case Grey:
            r = (Uint8)(intensity * 0.5);
            g = (Uint8)(intensity * 0.5);
            b = (Uint8)(intensity * 0.5);
            break;
        case DarkRed:
            r = (Uint8)(intensity * 0.8);
            g = 0;
            b = 0;
            break;
        case DarkGreen:
            r = 0;
            g = (Uint8)(intensity * 0.8);
            b = 0;
            break;
        case DarkBlue:
            r = 0;
            g = 0;
            b = (Uint8)(intensity * 0.8);
            break;
        case DarkYellow:
            r = (Uint8)(intensity * 0.6);
            g = (Uint8)(intensity * 0.6);
            b = 0;
            break;
        case DarkOrange:
            r = (Uint8)(intensity * 0.8);
            g = (Uint8)(intensity * 0.4);
            b = (Uint8)(intensity * 0.1);
            break;
        case DarkViolet:
            r = (Uint8)(intensity * 0.6);
            g = 0;
            b = (Uint8)(intensity * 0.6);
            break;
        case DarkBrown:
            r = (Uint8)(intensity * 0.3);
            g = (Uint8)(intensity * 0.2);
            b = (Uint8)(intensity * 0.1);
            break;
        case DarkGrey:
            r = (Uint8)(intensity * 0.3);
            g = (Uint8)(intensity * 0.3);
            b = (Uint8)(intensity * 0.3);
            break;
        case LightRed:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGreen:
            r = (Uint8)(intensity * 0.45);
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightBlue:
            r = (Uint8)(intensity * 0.45);
            b = intensity;
            g = (Uint8)(intensity * 0.45);
            break;
        case LightYellow:
            r = intensity;
            g = intensity;
            b = (Uint8)(intensity * 0.45);
            break;
        case LightOrange:
            r = intensity;
            g = (Uint8)(intensity * 0.75);
            b = (Uint8)(intensity * 0.35);
            break;
        case LightViolet:
            r = intensity;
            g = (Uint8)(intensity * 0.45);
            b = intensity;
            break;
        case LightBrown:
            r = intensity;
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.45);
            break;
        case LightGrey:
            r = (Uint8)(intensity * 0.85);
            g = (Uint8)(intensity * 0.85);
            b = (Uint8)(intensity * 0.85);
            break;
        case Black:
            r = (Uint8)(intensity * 0.15);
            g = (Uint8)(intensity * 0.15);
            b = (Uint8)(intensity * 0.15);
            break;
        case White:
            r = intensity;
            g = intensity;
            b = intensity;
            break;
        case Tan:
            r = intensity;
            g = (Uint8)(intensity * 0.9);
            b = (Uint8)(intensity * 0.6);
            break;
        case Gold:
            r = (Uint8)(intensity * 0.9);
            g = (Uint8)(intensity * 0.8);
            b = (Uint8)(intensity * 0.3);
            break;
        case Silver:
            r = (Uint8)(intensity * 0.99);
            g = (Uint8)(intensity * 0.99);
            b = intensity;
            break;
        case YellowGreen:
            r = (Uint8)(intensity * 0.5);
            g = (Uint8)(intensity * 0.8);
            b = (Uint8)(intensity * 0.3);
            break;
        case Cyan:
            r = 0;
            g = (Uint8)(intensity * 0.9);
            b = (Uint8)(intensity * 0.9);
            break;
        case Magenta:
            r = (Uint8)(intensity * 0.8);
            g = 0;
            b = (Uint8)(intensity * 0.8);
            break;
		default:
			r = 0;
			g = (Uint8)(intensity * 0.9);
			b = (Uint8)(intensity * 0.9);
			break;
    }
	color = SDL_MapRGB(screen->format, r, g, b);
    
	return color;
}

void SwapSprite(SDL_Surface *sprite, int color1, int color2, int color3)
{
    int x, y;
	SDL_Surface *temp;
    Uint32 pixel, pixel2;
    Uint32 Key = sprite->format->colorkey;
	
	if(color1 == -1)return;
    if(sprite == NULL)return;
    temp = SDL_DisplayFormatAlpha(sprite);
    if (SDL_LockSurface(temp) < 0)
    {
        fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
        exit(1);
    }

    for(y = 0; y < sprite->h; y++)
    {
        for(x = 0; x < sprite->w;x++)
        {
            pixel = getpixel(temp, x, y);/*and swap it*/
			if(pixel != Key)
			{
			  pixel2 = SetColor(pixel, color1, color2, color3);
              putpixel(sprite, x, y, pixel2);
			}
        }
    }
    SDL_UnlockSurface(temp);
	SDL_FreeSurface(temp);
}

Uint32 IndexColor(int color)
{
    switch(color)
    {
		case Red:
			return SDL_MapRGB(screen->format,168,0,0);
		case Green:
	        return SDL_MapRGB(screen->format,0,168,0);
		case Blue:
			return SDL_MapRGB(screen->format,0,0,168);
		case Yellow:
			return SDL_MapRGB(screen->format,168,168,0);
		case Orange:
			return SDL_MapRGB(screen->format,168,138,0);
		case Violet:
			return SDL_MapRGB(screen->format,168,0,168);
		case Brown:
			return SDL_MapRGB(screen->format,120,84,24);
		case Grey:
			return SDL_MapRGB(screen->format,128,128,128);
		case DarkRed:
			return SDL_MapRGB(screen->format,128,0,0);
		case DarkGreen:
			return SDL_MapRGB(screen->format,0,128,0);
		case DarkBlue:
			return SDL_MapRGB(screen->format,0,0,128);
		case DarkYellow:
			return SDL_MapRGB(screen->format,96,96,0);
		case DarkOrange:
			return SDL_MapRGB(screen->format,128,96,24);
		case DarkViolet:
			return SDL_MapRGB(screen->format,96,0,96);
		case DarkBrown:
			return SDL_MapRGB(screen->format,96,64,16);
		case DarkGrey:
			return SDL_MapRGB(screen->format,64,64,64);
		case LightRed:
			return SDL_MapRGB(screen->format,255,64,64);
		case LightGreen:
			return SDL_MapRGB(screen->format,64,255,64);
		case LightBlue:
			return SDL_MapRGB(screen->format,64,64,255);
		case LightYellow:
			return SDL_MapRGB(screen->format,250,250,96);
		case LightOrange:
			return SDL_MapRGB(screen->format,255,244,64);
		case LightViolet:
			return SDL_MapRGB(screen->format,250,64,250);
		case LightBrown:
			return SDL_MapRGB(screen->format,220,110,64);
		case LightGrey:
			return SDL_MapRGB(screen->format,196,196,196);
		case Black:
			return SDL_MapRGB(screen->format,1,1,1);
		case White:
			return SDL_MapRGB(screen->format,255,255,255);
		case Tan:
			return SDL_MapRGB(screen->format,255,128,64);
		case Gold:
			return SDL_MapRGB(screen->format,255,250,64);
		case Silver:
			return SDL_MapRGB(screen->format,245,245,255);
		case YellowGreen:
			return SDL_MapRGB(screen->format,225,255,64);
		case Cyan:
			return SDL_MapRGB(screen->format,0,255,255);
		case Magenta:
			return SDL_MapRGB(screen->format,255,0,255);
    }
    return SDL_MapRGB(screen->format,0,0,0);
}


void BlankScreen(SDL_Surface *buf, Uint32 color)
{
    SDL_LockSurface(buf);
    memset(buf->pixels, (Uint8)color, buf->format->BytesPerPixel * buf->w *buf->h);
    SDL_UnlockSurface(buf);
}

void FrameDelay(Uint32 delay)
{
	static Uint32 pass = 100;
	Uint32 dif;
	dif = SDL_GetTicks() - pass;
	if(dif < delay)SDL_Delay(delay - dif);
	pass = SDL_GetTicks();
}

void ResetBuffer()
{
	SDL_BlitSurface(background, &Camera, screen, NULL);
}

void NextFrame()
{
	Uint32 Then;
	SDL_BlitSurface(screen, NULL, videobuffer, NULL);
	SDL_Flip(videobuffer);
	FrameDelay(30);
	Then = NOW;
	NOW = SDL_GetTicks();
}

/*
Sprite *NewWindow(int sx, int sy, int sw, int sh, int bgcolor)
{
	int i;
	SDL_Surface *temp;
	if(NumWindows + 1 >= MaxWindows)
	{
		fprintf(stderr, "Maximum Windows Reached.\n");
		exit(1);
	}
	NumWindows++;
	for(i = 0; i <= NumWindows; i++)
	{
		if(!WindowList[i].used)break;
	}
	temp = SDL_CreateRGBSurface(0, sw, sh, S_Data.depth, rmask, gmask, bmask, amask);
	WindowList[i].image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	WindowList[i].w = sw;
	WindowList[i].h = sh;
	WindowList[i].color2 = sx;
	WindowList[i].color3 = sy;
	WindowList[i].color1 = bgcolor;
	WindowList[i].used = 1;
	PaintWindow(WindowList[1].image, 0, 0, sw - 1, sh - 1, bgcolor);
	return &WindowList[i];
}

void CloseWindow()
{
	int i;
	for(i = 0; i < MaxWindows; i++)
	{
		FreeSprite(&WindowList[i]);
	}
}

void DrawWindow(Sprite *sprite, SDL_Surface *surface)
{
	SDL_Rect src, dest;
	src.x = 0;
	src.y = 0;
	src.w = sprite->w;
	src.h = sprite->h;
	dest.x = sprite->color2;
	dest.y = sprite->color3;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

Sprite *DisplayBar()
{
	Sprite *win;
	SDL_Surface *txt;
	SDL_Surface *tmp;
	SDL_Color c1,c2;
	SDL_Rect dest;
	win = NewWindow(0, screen->h - 32, screen->w, 32, SDL_MapRGB(screen->format, 0, 0, 0));
	c1.r = 240;
	c1.g = 240;
	c1.b = 240;
	c2.r = 1;
	c2.g = 1;
	c2.b = 1;
	dest.x = (screen->w/2) - 46;
	dest.y = 8;
	tmp = TTF_RenderText_Shaded(font, "Flight", c1, c2);
	txt = SDL_DisplayFormat(tmp);
	SDL_SetColorKey(txt, SDL_SRCCOLORKEY, SDL_MapRGB(txt->format, 0, 0, 0));
	SDL_FreeSurface(tmp);
	SDL_BlitSurface(txt, NULL, win->image, &dest);
	return win;
}

void UpdateDisplayBar(Entity *player, Sprite *window)
{
	int i;
	SDL_Surface *temp;
	SDL_Surface *draw;
	SDL_Rect healthbar;
	healthbar.x = 130;
	healthbar.y = 8;
	healthbar.w = 100;
	healthbar.h = 16;
	temp = SDL_CreateRGBSurface(SDL_ANYFORMAT, window->w, window->h, S_Data.depth, rmask, gmask, bmask, amask);
	if(temp == NULL)
	{
		fprintf(stderr, "Couldn't create SDL Surface: %s\n", SDL_GetError());
		exit(1);
	}
	draw = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_BlitSurface(window->image, NULL, draw, NULL);
	for(i = 0; i < NumLives; i++)
		DrawSprite(player->sprite, draw, 5 + (i * player->sprite->w), 1, 0);
	SDL_FillRect(draw, &healthbar, SDL_MapRGB(screen->format, 255, 0, 0));
	healthbar.w = (player->health * 100) / player->healthmax;
	SDL_FillRect(draw, &healthbar, SDL_MapRGB(screen->format, 0, 255, 0));
	temp = window->image;
	window->image = draw;
	DrawWindow(window, screen);
	window->image = temp;
	SDL_FreeSurface(draw);
}

void ShowBMP(SDL_Surface *image, SDL_Surface *screen, int x, int y)
{
    SDL_Rect dest;

    // Blit onto the screen surface.
    //   The surfaces should not be locked at this point.
     
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, screen, &dest);

    // Update the changed portion of the screen
    SDL_UpdateRects(screen, 1, &dest);
}

void InitMouse()
{
	Mouse = NewEntity();
	Mouse->sprite = LoadSprite("images/mouse.png", 16, 16, White, Brown, Cyan);
	Mouse->framestates[0] = 0;
	Mouse->framestates[1] = 16;
	Mouse->framestates[2] = 32;
	Mouse->framestates[3] = 48;
	Mouse->state = 0;
	Mouse->shown = 0;
	Mouse->frame = 0;
}

void DrawMouse()
{
	int mx,my;
	SDL_GetMouseState(&mx, &my);
	DrawSprite(Mouse->sprite, screen, mx, my, Mouse->frame);
	Mouse->count = GetNextCount(Mouse);
	Mouse->frame = Mouse->count + Mouse->framestates[Mouse->state];
}
*/