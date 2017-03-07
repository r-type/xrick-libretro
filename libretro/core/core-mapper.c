#include "libretro.h"
#include "libretro-core.h"
#include "sdl_primitives.h"

#include "SDL.h"
//CORE VAR
#ifdef _WIN32
char slash = '\\';
#else
char slash = '/';
#endif
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

long frame=0;
unsigned long  Ktime=0 , LastFPSTime=0;

//VIDEO
//extern SDL_Surface *sdlscrn; 

#ifdef  RENDER16B
	uint16_t Retro_Screen[1600*1200];
#else
	unsigned int Retro_Screen[1600*1200];
#endif 

//SOUND
short signed int SNDBUF[1024*2];
int snd_sampler = 44100 / 50;

//PATH
char RPATH[512];

//EMU FLAGS
int MAXPAS=6,SHIFTON=-1,MOUSE_EMULATED=-1,PAS=4;
int SND; //SOUND ON/OFF
static int firstps=0;
int pauseg=0; //enter_gui

//JOY
int al[2][2];//left analog1
int ar[2][2];//right analog1
unsigned char MXjoy[2]; // joy
int NUMjoy=1;

//MOUSE
extern int pushi;  // gui mouse btn
int gmx,gmy; //gui mouse
int touch;
int fmousex,fmousey; //gui mouse

//KEYBOARD
char Key_Sate[512];
char Key_Sate2[512];

static int mbt[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//STATS GUI
//extern int LEDA,LEDB,LEDC;
int BOXDEC= 32+2;
int STAT_BASEY;

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
{ // in MSec
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

//NO SURE FIND BETTER WAY TO COME BACK IN MAIN THREAD IN HATARI GUI
void gui_poll_events(void)
{
   Ktime = GetTicks();

   if(Ktime - LastFPSTime >= 1000/50)
   {
      frame++; 
      LastFPSTime = Ktime;		
      co_switch(mainThread);
   }
}

void texture_uninit(void)
{
	SDL_Uninit();
}

void texture_init(void)
{
   memset(Retro_Screen, 0, sizeof(Retro_Screen));

   gmx=(retrow/2)-1;
   gmy=(retroh/2)-1;
}

#include "system.h"
#include "control.h"

#define SETBIT(x,b) x |= (b)
#define CLRBIT(x,b) x &= ~(b)
void retro_key_down(unsigned short retrok)
{
   //IKBD_PressSTKey(retrok,1); 
unsigned short key=retrok;

 if (key == syskbd_up || key == SDLK_UP) {
      SETBIT(control_status, CONTROL_UP);
      control_last = CONTROL_UP;
    }
    else if (key == syskbd_down || key == SDLK_DOWN) {
      SETBIT(control_status, CONTROL_DOWN);
      control_last = CONTROL_DOWN;
    }
    else if (key == syskbd_left || key == SDLK_LEFT) {
      SETBIT(control_status, CONTROL_LEFT);
      control_last = CONTROL_LEFT;
    }
    else if (key == syskbd_right || key == SDLK_RIGHT) {
      SETBIT(control_status, CONTROL_RIGHT);
      control_last = CONTROL_RIGHT;
    }
    else if (key == syskbd_pause) {
      SETBIT(control_status, CONTROL_PAUSE);
      control_last = CONTROL_PAUSE;
    }
    else if (key == syskbd_end) {
      SETBIT(control_status, CONTROL_END);
      control_last = CONTROL_END;
    }
    else if (key == syskbd_xtra) {
      SETBIT(control_status, CONTROL_EXIT);
      control_last = CONTROL_EXIT;
    }
    else if (key == syskbd_fire) {
      SETBIT(control_status, CONTROL_FIRE);
      control_last = CONTROL_FIRE;
    }
}

void retro_key_up(unsigned short retrok)
{
unsigned short key=retrok;
   //IKBD_PressSTKey(retrok,0);
 if (key == syskbd_up || key == SDLK_UP) {
      CLRBIT(control_status, CONTROL_UP);
      control_last = CONTROL_UP;
    }
    else if (key == syskbd_down || key == SDLK_DOWN) {
      CLRBIT(control_status, CONTROL_DOWN);
      control_last = CONTROL_DOWN;
    }
    else if (key == syskbd_left || key == SDLK_LEFT) {
      CLRBIT(control_status, CONTROL_LEFT);
      control_last = CONTROL_LEFT;
    }
    else if (key == syskbd_right || key == SDLK_RIGHT) {
      CLRBIT(control_status, CONTROL_RIGHT);
      control_last = CONTROL_RIGHT;
    }
    else if (key == syskbd_pause) {
      CLRBIT(control_status, CONTROL_PAUSE);
      control_last = CONTROL_PAUSE;
    }
    else if (key == syskbd_end) {
      CLRBIT(control_status, CONTROL_END);
      control_last = CONTROL_END;
    }
    else if (key == syskbd_xtra) {
      CLRBIT(control_status, CONTROL_EXIT);
      control_last = CONTROL_EXIT;
    }
    else if (key  == syskbd_fire) {
      CLRBIT(control_status, CONTROL_FIRE);
      control_last = CONTROL_FIRE;
    }
    
}

#include "sdl-wrapper.c"
extern  const char STScanCode[SDLK_LAST] ;

int bitstart=0;
int pushi=0; //mouse button
int keydown=0,keyup=0;
int KBMOD=-1;
int SurfaceFormat=3;

void Process_key(void)
{
   int i;

   keydown=0;keyup=0;

   for(i=0;i<320;i++)
   {
      Key_Sate[i]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
      
         if(Key_Sate[i]  && Key_Sate2[i]==0)
         {
#if 0
if(i==308/*SDLK_RALT*/){
	KBMOD=-KBMOD;
	printf("Modifier pressed %d \n",KBMOD); 
        Key_Sate2[i]=1;
	
	continue;
}
#endif
  SDL_keysym keysym;

  keysym.scancode=i;
  keysym.sym=i;
  keysym.unicode=0;
  if(KBMOD==1)keysym.mod=0x0200;
  else keysym.mod=0;

//Keymap_KeyDown(&keysym);

            retro_key_down(i);
            Key_Sate2[i]=1;
bitstart=1;//
keydown++;
         }
         else if ( !Key_Sate[i] && Key_Sate2[i]==1 )
         {
#if 0
if(i==308/*SDLK_RALT*/){
	//KBMOD=-KBMOD;
	//printf("Modifier pressed %d \n",KBMOD); 
        Key_Sate2[i]=0;
	
	continue;
}
#endif
  SDL_keysym keysym;

  keysym.scancode=i;
  keysym.sym=i;
  keysym.unicode=0;
  keysym.mod=0;

//Keymap_KeyUp(&keysym);

            retro_key_up( i );
            Key_Sate2[i]=0;
bitstart=0;
keyup++;

         }
      
   }



}
//#include "ikbd.h"
int Retro_PollEvent(void)
{
   int SAVPAS=PAS;	

   input_poll_cb();

   int mouse_l;
   int mouse_r;
   int16_t mouse_x,mouse_y;
   mouse_x=mouse_y=0;

   Process_key();

   //mouse/joy toggle
   if ( input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, 2) && mbt[2]==0 )
      mbt[2]=1;
   else if ( mbt[2]==1 && ! input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, 2) ){
      mbt[2]=0;
      MOUSE_EMULATED=-MOUSE_EMULATED;
   }

   if(MOUSE_EMULATED==1){

      //TODO FIX THIS :(
#if defined(__CELLOS_LV2__) 
      //Slow Joypad Mouse Emulation for PS3
      static int pair=-1;
      pair=-pair;
      if(pair==1)return;
      PAS=1;
#elif defined(GEKKO) 
      PAS=1;
#endif

      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))mouse_x += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))mouse_x -= PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))mouse_y += PAS;
      if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))mouse_y -= PAS;
      mouse_l=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
      mouse_r=input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);

      PAS=SAVPAS;
   }
   else {

      mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
      mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
      mouse_l    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
      mouse_r    = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
   }

   static int mmbL=0,mmbR=0;

   if(mmbL==0 && mouse_l){

      mmbL=1;		
      pushi=1;
    // Keyboard.bLButtonDown |= BUTTON_MOUSE;
   }
   else if(mmbL==1 && !mouse_l) {

      mmbL=0;
      pushi=0;
    //  Keyboard.bLButtonDown &= ~BUTTON_MOUSE;
   }

   if(mmbR==0 && mouse_r){
      mmbR=1;
     // Keyboard.bRButtonDown |= BUTTON_MOUSE;		
   }
   else if(mmbR==1 && !mouse_r) {
      mmbR=0;
     // Keyboard.bRButtonDown &= ~BUTTON_MOUSE;
   }

  fmousex=mouse_x;
  fmousey=mouse_y;
 // Main_HandleMouseMotion();

   gmx+=mouse_x;
   gmy+=mouse_y;
   if(gmx<0)gmx=0;
   if(gmx>retrow-1)gmx=retrow-1;
   if(gmy<0)gmy=0;
   if(gmy>retroh-1)gmy=retroh-1;


return 1;

}
