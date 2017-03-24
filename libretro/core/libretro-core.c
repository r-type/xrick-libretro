#include "libretro.h"

#include "libretro-core.h"

#include "game.h"
#include "system.h"

int VIRTUAL_WIDTH ;
int retrow=320; 
int retroh=200;
#ifdef RENDER16B
#define BPP 2
#else
#define BPP 4
#endif

extern int SND;
extern char RPATH[512];
extern char RETRO_DIR[512];

SDL_Surface *sdlscrn; 

void SDL_Uninit(void)
{
   if(sdlscrn->format->palette->colors)	
      free(sdlscrn->format->palette->colors);

   if(sdlscrn->format->palette)	
      free(sdlscrn->format->palette);
   if(sdlscrn->format)	
      free(sdlscrn->format);

   if(sdlscrn->pixels)
      sdlscrn->pixels=NULL;

   if(sdlscrn)	
      free(sdlscrn);
}

const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;

#include "cmdline.c"

extern void texture_init(void);
extern void texture_uninit(void);

static retro_video_refresh_t video_cb;
retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   struct retro_variable variables[] = {
      { NULL, NULL },
   };

   bool no_content = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_content);

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

static void update_variables(void)
{
}

static void retro_wrap_emulator(void)
{    
}

void retro_reset(void)
{
}

void retro_init(void)
{    	
   const char *system_dir  = NULL;
   const char *content_dir = NULL;
   const char *save_dir    = NULL;

   // if defined, use the system directory			
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
      retro_system_directory=system_dir;		

   // if defined, use the system directory			
   if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
      retro_content_directory=content_dir;		

   // If save directory is defined use it, otherwise use system directory
   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
      retro_save_directory = *save_dir ? save_dir : retro_system_directory;      
   else
      // make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
      retro_save_directory=retro_system_directory;

   if(retro_system_directory==NULL)
      sprintf(RETRO_DIR, "%s\0",".");
   else
      sprintf(RETRO_DIR, "%s\0", retro_system_directory);

   printf("Retro SYSTEM_DIRECTORY %s\n",retro_system_directory);
   printf("Retro SAVE_DIRECTORY %s\n",retro_save_directory);
   printf("Retro CONTENT_DIRECTORY %s\n",retro_content_directory);

#ifndef RENDER16B
   enum retro_pixel_format fmt =RETRO_PIXEL_FORMAT_XRGB8888;
#else
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
#endif
   
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      fprintf(stderr, "PIXEL FORMAT is not supported.\n");
      exit(0);
   }

   memset(Key_Sate,0,512);
   memset(Key_Sate2,0,512);

   texture_init();
}

void retro_deinit(void)
{	 
   texture_uninit();
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "xrick";
   info->library_version  = "021212-Dev";
   info->valid_extensions = "*|zip";
   info->need_fullpath    = true;
   info->block_extract = false;

}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   struct retro_game_geometry geom = { retrow, retroh, retrow, retrow,4.0 / 3.0 };
   struct retro_system_timing timing = { 50.0, 22050.0 };

   info->geometry = geom;
   info->timing   = timing;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

extern int Retro_PollEvent(void);
extern void syssnd_callback(U8 *stream, int len);

void retro_run(void)
{
   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   Retro_PollEvent();

   game_iterate();

   if(sdlscrn)
      video_cb(sdlscrn->pixels,retrow,retroh, retrow<< PIXEL_BYTES);
}

bool retro_load_game(const struct retro_game_info *info)
{
   const char *full_path = info->path;

   update_variables();

#ifdef RENDER16B
   memset(Retro_Screen,0,WINDOW_WIDTH*WINDOW_HEIGHT*2);
   SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT, 16, 0);
#else
   memset(Retro_Screen,0,WINDOW_WIDTH*WINDOW_HEIGHT*2*2);
   sdlscrn = SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT ,32, 0);
#endif

   SND=1;
   sprintf(RPATH,"\"xrick\" \"-data\" \"%s/data.zip\"\0",retro_system_directory);
   pre_main(RPATH);
	game_run();

   return true;
}

void freedata(void);

void retro_unload_game(void)
{
	freedata(); /* free cached data */
	data_closepath();
	sys_shutdown();
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void) {}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

