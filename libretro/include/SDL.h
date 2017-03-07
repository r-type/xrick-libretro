
#ifndef HATARI_SDL_H
#define HATARI_SDL_H

//RETRO HACK
//#warning This is just an SDL wrapper for the retro core.

extern int Reset_Cold(void);
extern int Reset_Warm(void);

#include <unistd.h>
#include <time.h>

#include "SDL_types.h"
#include "SDL_keyboard.h"
#include "SDL_video.h"

#define SDL_GetTicks  GetTicks 
#include <SDL_endian.h>
#include <SDL_types.h>

#define SDL_SRCCOLORKEY 0x1
#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))
extern int Retro_SetPalette(SDL_Surface * surface, int flags, const SDL_Color * colors,int firstcolor, int ncolors);
extern unsigned int Retro_MapRGB(SDL_PixelFormat *a, int r, int g, int b);
extern long GetTicks(void);
extern SDL_Surface *Retro_CreateRGBSurface( int w,int h, int d, int rm,int rg,int rb,int ra);
extern SDL_Surface *Retro_SetVideoMode(int w,int h,int b);
extern void Retro_FreeSurface(SDL_Surface *surf );
extern void Retro_BlitSurface(SDL_Surface *ss,SDL_Rect* sr,SDL_Surface *ds,SDL_Rect* dr);
extern void Retro_Fillrect(SDL_Surface * surf,SDL_Rect *rect,unsigned int col);
extern int SDL_SaveBMP(SDL_Surface *surface,const char *file);
extern SDL_Surface * SDL_LoadBMP(const char *file);
extern void Retro_GetRGB(int coul,SDL_PixelFormat *format, int *r,int *g,int *b);
extern int Retro_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
extern int Retro_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);

#define SDL_MapRGB(a, r, g, b) Retro_MapRGB(a, r, g, b)

typedef struct SDL_Event{
Uint8 type;
} SDL_Event;

//SOME SDL_FUNC WRAPPER
//GLOBALS
#define SDL_ShowCursor(a) 0
#define SDL_GRAB_OFF 0
#define SDL_GRAB_ON 1
#define SDL_WM_GrabInput(a)
#define SDL_WM_IconifyWindow()
#define SDL_WM_SetCaption(...)
#define SDL_HWSURFACE 0
#define SDL_FULLSCREEN 1
#define SDL_SWSURFACE 2
#define SDL_HWPALETTE 4
#define SDL_INIT_NOPARACHUTE 1
#define SDL_DISABLE 0
#define SDL_Quit(...)
#define SDL_InitSubSystem(...) 0
#define SDL_Init(...) 0
#define SDL_INIT_JOYSTICK 0
#define SDL_PHYSPAL 0
#define SDL_DOUBLEBUF 0
#define SDL_RESIZABLE 0
#define SDL_BUTTON(a) 1
#define SDL_JoystickUpdate() 
#define SDL_WM_SetIcon(a,b) 

#define    SDL_KEYDOWN  0x300
#define    SDL_KEYUP    0x301
//TIME
#define SDL_Delay(a) usleep((a)*1000)
//SURFACE
#define SDL_SetColors(a, b, c, d) Retro_SetColors(a, b, c, d)
#define SDL_MUSTLOCK(a) 0
#define SDL_LockSurface(a) 0
#define SDL_UnlockSurface(a) 0
#define SDL_FillRect(s,r,c) Retro_Fillrect((s),(r),(c))
#define SDL_UpdateRects(a, b,c)
#define SDL_UpdateRect(...)
#define SDL_SetVideoMode(w, h, b, f) Retro_SetVideoMode((w),(h),(b))
#define SDL_FreeSurface(a) Retro_FreeSurface((a))
#define SDL_CreateRGBSurface(a,w,h,d,rm,gm,bm,am) Retro_CreateRGBSurface((w),(h),(d),(rm),(gm),(bm),(am))
#define SDL_BlitSurface(ss,sr,ds,dr) Retro_BlitSurface((ss),(sr),(ds),(dr))
#define SDL_GetRGB(clr, format, r, g, b)  Retro_GetRGB((clr), (format), (r),(g),(b))
#define SDL_SetColorKey(a, b,c)  Retro_SetColorKey((a),(b),(c))
#define SDL_SetPalette(s, f,c,d,n) Retro_SetPalette((s),(f),(c),(d),(n))

//KEY
#define SDL_GetError() "RetroWrapper"
#define SDL_GetModState() 0
#define SDL_GetKeyName(...) "RetroWrapper"
//EVENT
#define SDL_PollEvent(a) Retro_PollEvent()
//SOUND
#define SDL_CloseAudio();
#define SDL_LockAudio();
#define SDL_UnlockAudio();
#define SDL_PauseAudio(a);
//JOY
#define SDL_Joystick int
#define SDL_NumJoysticks() 0
#define SDL_JoystickOpen(i) NULL
#define SDL_JoystickName(i) "RetroWrapper"
#define SDL_JoystickClose
#define SDL_JoystickGetAxis(...) 0
#define SDL_JoystickGetButton(...) 0
#define SDL_JoystickNumAxes(...) 0
#define SDL_JoystickNumButtons(a) 16
//MOUSE
#define SDL_GetMouseState(X,Y) GuiGetMouseState((X),(Y))
#define SDL_WarpMouse(x, y) do {gmx=x;gmy=y;} while (0) 

//PS3 HACK
#if defined(__CELLOS_LV2__) 
#include <unistd.h> //stat() is defined here
#define S_ISDIR(x) (x & CELL_FS_S_IFDIR)
#define F_OK 0

#include "sys/sys_time.h"
#include "sys/timer.h"
#define usleep  sys_timer_usleep
#endif


#endif
