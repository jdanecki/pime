#ifndef __BACKEND_INL__
#define __BACKEND_INL__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_render.h>
#include <SDL_mixer.h>

typedef SDL_Texture * Backend_Texture;
typedef SDL_TimerID timer_id;
typedef SDL_TimerCallback timer_callback;
typedef SDL_Color Backend_Color;
typedef TTF_Font * Backend_Font;
typedef SDL_Surface * Backend_Surface;
typedef int Backend_Rect_Field;

class Backend_Rect
{
public:
    SDL_Rect r;
    Backend_Rect(Backend_Rect_Field x,Backend_Rect_Field y,Backend_Rect_Field w, Backend_Rect_Field h)
    {
        r.x=x;
        r.y=y;
        r.w=w;
        r.h=h;
    }
    int get_w() { return r.w; }
    int get_h() { return r.h; }
    void set_w(int w) { r.w = w; }
    void set_h(int h) { r.h = h; }
    void add_w(int w) { r.w += w; }
    void add_h(int h) { r.h += h; }
};

struct Backend_Pixels
{
    SDL_Texture * texture;
    unsigned char * pixels;
    int pitch;
};

extern SDL_Renderer * renderer;
extern SDL_Window * main_window;

#define Backend_Image_Load IMG_Load
#define Backend_Check_Surface(surf) (surf != NULL)
#define Backend_Check_Texture(tex) (tex != NULL)
#define Backend_Free_Surface(img) SDL_FreeSurface(img)
#define Backend_Begin_Drawing()
#define Backend_End_Drawing()

SDL_Texture * Backend_Create_Texture_From_Surface(SDL_Surface * image);
const char * Backend_Get_Error();
void Backend_Wait();
void Backend_Update_Screen();
void Backend_Texture_Copy_With_Mask(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect, SDL_Color color, bool mask);
void Backend_Texture_Copy(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect);
void Backend_Texture_Copy_Flip(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect);
int Backend_Get_Texture_Size(SDL_Texture *texture, int *w, int *h);
Backend_Pixels Backend_Allocate_Pixels(int w, int h);
void Backend_Map_Pixels(Backend_Pixels pixels);
void Backend_Update_Texture_Pixels(Backend_Pixels pixels);

void Backend_Draw_Fill_Rectangle(Backend_Rect r, SDL_Color color);
void Backend_Draw_Rectangle(Backend_Rect r, SDL_Color color);
void Backend_Draw_Gradient_Rectangle(int x, int y, int w, int h, SDL_Color top, SDL_Color bottom);

void Backend_Window_Size(int *w, int *h);
int Backend_Set_Clip(int x, int y, int w, int h);

int CONSOLE_LOG(const char * fmt, ...);
#endif
