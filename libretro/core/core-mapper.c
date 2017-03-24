#include "libretro.h"
#include "libretro-core.h"
#include "sdl_primitives.h"

#include "SDL.h"
extern const char *retro_save_directory;
extern const char *retro_system_directory;
extern const char *retro_content_directory;
char RETRO_DIR[512];
 
//TIME
#ifdef __CELLOS_LV2__
#include "sys/sys_time.h"
#include "sys/timer.h"
#define usleep  sys_timer_usleep
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#endif

#include "system.h"
#include "control.h"

#define SETBIT(x,b) x |= (b)
#define CLRBIT(x,b) x &= ~(b)

//VIDEO
#ifdef  RENDER16B
uint16_t Retro_Screen[WINDOW_WIDTH*WINDOW_HEIGHT];
#else
unsigned int Retro_Screen[WINDOW_WIDTH*WINDOW_HEIGHT];
#endif 

//PATH
char RPATH[512];

//EMU FLAGS
int SND; //SOUND ON/OFF

//JOY
int al[2][2];//left analog1
int ar[2][2];//right analog1

//KEYBOARD
char Key_Sate[512];
char Key_Sate2[512];

static retro_input_state_t input_state_cb;
static retro_input_poll_t input_poll_cb;

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

long GetTicks(void)
{
   // in MSec
#ifndef _ANDROID_

#ifdef __CELLOS_LV2__

   //#warning "GetTick PS3\n"

   unsigned long	ticks_micro;
   uint64_t secs;
   uint64_t nsecs;

   sys_time_get_current_time(&secs, &nsecs);
   ticks_micro =  secs * 1000000UL + (nsecs / 1000);

   return ticks_micro/1000;
#else
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return (tv.tv_sec*1000000 + tv.tv_usec)/1000;
#endif

#else

   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);
   return (now.tv_sec*1000000 + now.tv_nsec/1000)/1000;
#endif

} 

void texture_uninit(void)
{
   SDL_Uninit();
}

void texture_init(void)
{
   memset(Retro_Screen, 0, sizeof(Retro_Screen));
}

static void retro_key_down(unsigned short key)
{
   switch (key)
   {
      case SDLK_UP:
         SETBIT(control_status, CONTROL_UP);
         control_last = CONTROL_UP;
         break;
      case SDLK_DOWN:
         SETBIT(control_status, CONTROL_DOWN);
         control_last = CONTROL_DOWN;
         break;
      case SDLK_LEFT:
         SETBIT(control_status, CONTROL_LEFT);
         control_last = CONTROL_LEFT;
         break;
      case SDLK_RIGHT:
         SETBIT(control_status, CONTROL_RIGHT);
         control_last = CONTROL_RIGHT;
         break;
      case SDLK_p:
         SETBIT(control_status, CONTROL_PAUSE);
         control_last = CONTROL_PAUSE;
         break;
      case SDLK_e:
         SETBIT(control_status, CONTROL_END);
         control_last = CONTROL_END;
         break;
      case SDLK_ESCAPE:
         SETBIT(control_status, CONTROL_EXIT);
         control_last = CONTROL_EXIT;
         break;
      case SDLK_SPACE:
         SETBIT(control_status, CONTROL_FIRE);
         control_last = CONTROL_FIRE;
         break;
   }
}

static void retro_key_up(unsigned short key)
{
   switch (key)
   {
      case SDLK_UP:
         CLRBIT(control_status, CONTROL_UP);
         control_last = CONTROL_UP;
         break;
      case SDLK_DOWN:
         CLRBIT(control_status, CONTROL_DOWN);
         control_last = CONTROL_DOWN;
         break;
      case SDLK_LEFT:
         CLRBIT(control_status, CONTROL_LEFT);
         control_last = CONTROL_LEFT;
         break;
      case SDLK_RIGHT:
         CLRBIT(control_status, CONTROL_RIGHT);
         control_last = CONTROL_RIGHT;
         break;
      case SDLK_p:
         CLRBIT(control_status, CONTROL_PAUSE);
         control_last = CONTROL_PAUSE;
         break;
      case SDLK_e:
         CLRBIT(control_status, CONTROL_END);
         control_last = CONTROL_END;
         break;
      case SDLK_ESCAPE:
         CLRBIT(control_status, CONTROL_EXIT);
         control_last = CONTROL_EXIT;
         break;
      case SDLK_SPACE:
         CLRBIT(control_status, CONTROL_FIRE);
         control_last = CONTROL_FIRE;
         break;
   }
}

#include "sdl-wrapper.c"

int SurfaceFormat=3;

#define key_latch(key) \
   if(Key_Sate[key]  && Key_Sate2[key]==0) \
   { \
      retro_key_down(key); \
      Key_Sate2[key] = 1; \
   } \
   else if ( !Key_Sate[key] && Key_Sate2[key]==1 ) \
   { \
      retro_key_up(key); \
      Key_Sate2[key] = 0; \
   }

int Retro_PollEvent(void)
{
   unsigned short key = 0;

   input_poll_cb();

   key           = SDLK_UP;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) ? 0x80: 0;
   key_latch(key);
   key           = SDLK_DOWN;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN) ? 0x80: 0;
   key_latch(key);
   key           = SDLK_LEFT;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT) ? 0x80: 0;
   key_latch(key);
   key           = SDLK_RIGHT;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT) ? 0x80: 0;
   key_latch(key);

   key           = SDLK_p;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START) ? 0x80: 0;
   key_latch(key);

#if 0
   key           = SDLK_e;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT) ? 0x80: 0;
   key_latch(key);
#endif

   key           = SDLK_SPACE;
   Key_Sate[key] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) ? 0x80: 0;
   key_latch(key);


   return 1;

}

