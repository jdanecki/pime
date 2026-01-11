#ifndef __BACKEND_INL__
#define __BACKEND_INL__

#include <raylib.h>

typedef Texture2D Backend_Texture;
typedef unsigned int timer_id;
typedef unsigned int (*timer_callback) (unsigned int interval, void *param);
typedef Color Backend_Color;
typedef Font Backend_Font;
typedef Image Backend_Surface;

typedef float Backend_Rect_Field;

class Backend_Rect
{
public:
    Rectangle r;
    Backend_Rect(Backend_Rect_Field x, Backend_Rect_Field y, Backend_Rect_Field w, Backend_Rect_Field h)
    {
        r.x=x;
        r.y=y;
        r.width=w;
        r.height=h;
    }

    float get_w() { return r.width; }
    float get_h() { return r.height; }
    void set_w(float w) { r.width = w; }
    void set_h(float h) { r.height = h; }
    void add_w(float w) { r.width += w; }
    void add_h(float h) { r.height += h; }
};

struct Backend_Pixels
{
    Texture2D texture;
    unsigned char * pixels;
    int pitch;
};

extern void Backend_Texture_Copy_With_Mask(Texture2D texture, Backend_Rect *srcrect, Backend_Rect *dstrect, Color color, bool mask);
extern void Backend_Texture_Copy(Texture2D texture, Backend_Rect *srcrect, Backend_Rect *dstrect);
extern void Backend_Texture_Copy_Flip(Texture2D texture, Backend_Rect *srcrect, Backend_Rect *dstrect);

#define Backend_Image_Load LoadImage
#define Backend_Check_Surface(surf) (surf.data != NULL)
#define Backend_Check_Texture(tex) (tex.id != 0)
#define Backend_Free_Surface(img) UnloadImage(img)
#define Backend_Begin_Drawing() BeginDrawing()
#define Backend_End_Drawing() EndDrawing()

//#define BeginBlendMode(BLEND_ALPHA)
//EndBlendMode(); 

extern Texture2D Backend_Create_Texture_From_Surface(Image image);
extern const char * Backend_Get_Error();
extern void Backend_Wait();
extern void Backend_Update_Screen();
extern int Backend_Get_Texture_Size(Texture texture, int *w, int *h);
extern Backend_Pixels Backend_Allocate_Pixels(int w, int h);
extern void Backend_Update_Texture_Pixels(Backend_Pixels pixels);
extern void Backend_Draw_Fill_Rectangle(Backend_Rect r, Color color);
extern void Backend_Draw_Rectangle(Backend_Rect r, Color color);
extern void Backend_Draw_Gradient_Rectangle(int x, int y, int w, int h, Color top, Color bottom);

extern void Backend_Window_Size(int *w, int *h);

int CONSOLE_LOG(const char * fmt, ...);
#endif

