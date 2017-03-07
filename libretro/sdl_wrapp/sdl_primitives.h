#ifndef GRAPH_H
#define GRAPH_H 1

#include "SDL.h"

void SDL_DrawPixel(SDL_Surface *buffer,int x, int y, unsigned  color);
void SDL_DrawFRect(SDL_Surface *buffer,int x,int y,int dx,int dy,unsigned  color);
void SDL_DrawRect(SDL_Surface  *buffer,int x,int y,int dx,int dy,unsigned   color);
void SDL_Drawline(SDL_Surface *buffer,int x1,int y1,int x2,int y2,unsigned  color);
void SDL_DrawCircle(SDL_Surface *buf,int x, int y, int radius,unsigned  rgba,int full);
void SDL_DrawString(SDL_Surface *surf, signed short int x, signed short int y, 
      const unsigned char *string,unsigned short maxstrlen,unsigned short xscale,
      unsigned short yscale, unsigned  fg, unsigned  bg);
void SDL_Draw_text(SDL_Surface *buffer,int x,int y,unsigned  fgcol,
      unsigned  int bgcol ,int scalex,int scaley , int max,char *string, ...);
#endif

