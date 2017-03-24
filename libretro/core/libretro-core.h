#ifndef LIBRETRO_CORE_H
#define LIBRETRO_CORE_H 1

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"

#include <stdbool.h>

#ifdef  RENDER16B
extern uint16_t Retro_Screen[1600*1200];
#define PIXEL_BYTES 1
#define PIXEL_TYPE uint16_t
#else
extern unsigned int Retro_Screen[1600*1200];
#define PIXEL_BYTES 2
#define PIXEL_TYPE uint32_t 
#endif 

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 240

#include "libco/libco.h"

extern cothread_t mainThread;
extern cothread_t emuThread;

extern char Key_Sate[512];
extern char Key_Sate2[512];

extern int pauseg; 

#include "SDL_video.h"

#define LOGI printf

#define RGB565(r, g, b)  (((r) << (5+6)) | ((g) << 6) | (b))

#endif
