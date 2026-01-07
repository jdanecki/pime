#ifndef __BACKEND_INL__
#define __BACKEND_INL__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>

typedef SDL_Texture * Backend_Texture;
typedef SDL_TimerID timer_id;
typedef SDL_TimerCallback timer_callback;
typedef SDL_Color Backend_Color;
typedef TTF_Font * Backend_Font;
typedef SDL_Surface * Backend_Surface;

extern SDL_Renderer * renderer;
extern SDL_Window * main_window;

#define Backend_Image_Load IMG_Load
#define Backend_Check_Surface(surf) (surf != NULL)
#define Backend_Check_Texture(tex) (tex != NULL)
#define Backend_Free_Surface(img) SDL_FreeSurface(img)

extern SDL_Texture * Backend_Create_Texture_From_Surface(SDL_Surface * image);
extern const char * Backend_Get_Error();
#endif
