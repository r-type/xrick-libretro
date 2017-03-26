#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "SDL.h"

#include "sdl_primitives.h"

#define VIRTUAL_WIDTH buffer->w
//pitch

void SDL_DrawPixel(SDL_Surface *buffer,int x, int y, unsigned  color)
{
   unsigned  *mbuffer=(unsigned *)buffer->pixels;
   int idx=x+y*VIRTUAL_WIDTH;
   mbuffer[idx]=color;
}

void SDL_DrawFRect(SDL_Surface *buffer,int x,int y,int dx,int dy,unsigned  color)
{
   int i,j,idx;

   unsigned  *mbuffer=(unsigned  *)buffer->pixels;

   for(i=x;i<x+dx;i++)
   {
      for(j=y;j<y+dy;j++)
      {
         idx=i+j*VIRTUAL_WIDTH;
         mbuffer[idx]=color;	
      }
   }
}

void SDL_DrawRect(SDL_Surface  *buffer,int x,int y,int dx,int dy,unsigned   color)
{
   int i,j,idx;

   unsigned  *mbuffer=(unsigned  *)buffer->pixels;

   for(i=x;i<x+dx;i++)
   {
      idx=i+y*VIRTUAL_WIDTH;
      mbuffer[idx]=color;
      idx=i+(y+dy)*VIRTUAL_WIDTH;
      mbuffer[idx]=color;
   }

   for(j=y;j<y+dy;j++)
   {
      idx=x+j*VIRTUAL_WIDTH;
      mbuffer[idx]=color;	
      idx=(x+dx)+j*VIRTUAL_WIDTH;
      mbuffer[idx]=color;	
   }
}


void SDL_DrawHline(SDL_Surface  *buffer,int x,int y,int dx,int dy,unsigned   color)
{
   int i,j,idx;

   unsigned  *mbuffer=(unsigned *)buffer->pixels;
   for(i=x;i<x+dx;i++)
   {
      idx=i+y*VIRTUAL_WIDTH;
      mbuffer[idx]=color;		
   }
}

void SDL_DrawVline(SDL_Surface *buffer,int x,int y,int dx,int dy,unsigned   color)
{
   int i,j,idx;

   unsigned  *mbuffer=(unsigned *)buffer->pixels;
   for(j=y;j<y+dy;j++)
   {
      idx=x+j*VIRTUAL_WIDTH;
      mbuffer[idx]=color;		
   }
}

void SDL_Drawline(SDL_Surface *buffer,int x1,int y1,int x2,int y2,unsigned  color)
{
   int pixx, pixy, x, y, swaptmp, idx;

   int dx = x2 - x1;
   int dy = y2 - y1;
   int sx = (dx >= 0) ? 1 : -1;
   int sy = (dy >= 0) ? 1 : -1;

   unsigned  *mbuffer=(unsigned *)buffer->pixels;

   if (dx==0)
   {
      if (dy>0)
         SDL_DrawVline(buffer, x1, y1,0, dy, color);
      else if (dy<0)
         SDL_DrawVline(buffer, x1, y2,0, -dy, color);
      else
      {
         idx=x1+y1*VIRTUAL_WIDTH;
         mbuffer[idx]=color;
      }
      return;
   }

   if (dy == 0)
   {
      if (dx > 0)
         SDL_DrawHline(buffer, x1, y1, dx, 0, color);
      else if (dx < 0)
         SDL_DrawHline(buffer, x2, y1, -dx,0, color);
      return;
   }

   dx = sx * dx + 1;
   dy = sy * dy + 1;

   pixx = 1;
   pixy = VIRTUAL_WIDTH;

   pixx *= sx;
   pixy *= sy;

   if (dx < dy)
   {
      swaptmp = dx;
      dx = dy;
      dy = swaptmp;
      swaptmp = pixx;
      pixx = pixy;
      pixy = swaptmp;
   }

   x = 0;
   y = 0;

   idx=x1+y1*VIRTUAL_WIDTH;

   for (; x < dx; x++, idx +=pixx)
   {
      mbuffer[idx]=color;
      y += dy;
      if (y >= dx)
      {
         y -= dx;
         idx += pixy;
      }
   }
}

const float DEG2RAD = 3.14159/180;

void SDL_DrawCircle(SDL_Surface *buffer,int x, int y, int radius,unsigned  rgba,int full)
{
   int i, x1, y1;
   float degInRad; 

   unsigned  *mbuffer=(unsigned *)buffer->pixels;

   for (i = 0; i < 360; i++)
   {
      degInRad = i*DEG2RAD;
      x1 = x + cos(degInRad) * radius;
      y1 = y + sin(degInRad) * radius;

      if (full)
         SDL_Drawline(buffer,x,y, x1,y1,rgba); 
      else
         mbuffer[x1+y1*VIRTUAL_WIDTH]=rgba;
   }
}

#include "font2.c"

void SDL_DrawString(SDL_Surface *surf, signed short int x, signed short int y, 
      const unsigned char *string,unsigned short maxstrlen,unsigned short xscale,
      unsigned short yscale, unsigned  fg, unsigned  bg)
{
   int k,strlen, col, bit, xrepeat, yrepeat, surfw, surfh;
   unsigned char *linesurf, b;
   signed  int ypixel;

   if(string == NULL)
      return;

   for(strlen = 0; strlen<maxstrlen && string[strlen]; strlen++)
   {}

   surfw=strlen * 7 * xscale;
   surfh=8 * yscale;

   unsigned  *mbuffer=(unsigned *)surf->pixels;
   unsigned  *yptr; 

   linesurf =malloc(sizeof(unsigned )*surfw*surfh );
   yptr = (unsigned *)&linesurf[0];


   for(ypixel = 0; ypixel<8; ypixel++)
   {

      for(col=0; col<strlen; col++)
      {

         b = font_array[(string[col]^0x80)*8 + ypixel];

         for(bit=0; bit<7; bit++, yptr++)
         {              
            *yptr = (b & (1<<(7-bit))) ? fg : bg;
            for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
               yptr[1] = *yptr;
         }
      }

      for(yrepeat = 1; yrepeat < yscale; yrepeat++) 
         for(xrepeat = 0; xrepeat<surfw; xrepeat++, yptr++)
            *yptr = yptr[-surfw];

   }


   yptr = (unsigned *)&linesurf[0];

   for(yrepeat = y; yrepeat < y+ surfh; yrepeat++) 
      for(xrepeat = x; xrepeat< x+surfw; xrepeat++,yptr++)
         if(*yptr!=0)mbuffer[xrepeat+yrepeat*surf->w] = *yptr;

   free(linesurf);
}


void SDL_Draw_text(SDL_Surface *buffer,int x,int y,unsigned  fgcol,
      unsigned  int bgcol ,int scalex,int scaley , int max,char *string, ...)
{
   int boucle=0;  
   char text[256];	   	
   va_list	ap;			

   if (string == NULL)
      return;

   va_start(ap, string);		
   vsprintf(text, string, ap);	
   va_end(ap);	

   SDL_DrawString(buffer, x,y, text,max, scalex, scaley,fgcol,bgcol);	
}






