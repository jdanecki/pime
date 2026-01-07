#ifndef __BACKEND_INL__
#define __BACKEND_INL__

#include <raylib.h>

typedef Texture2D Backend_Texture;
typedef unsigned int timer_id;
typedef unsigned int (*timer_callback) (unsigned int interval, void *param);
typedef Color Backend_Color;
typedef Font Backend_Font;
typedef Image Backend_Surface;

#define Backend_Image_Load LoadImage
#define Backend_Check_Surface(surf) (surf.data != NULL)
#define Backend_Check_Texture(tex) (tex.id != 0)
#define Backend_Free_Surface(img) UnloadImage(img)

extern Texture2D Backend_Create_Texture_From_Surface(Image image);
extern const char * Backend_Get_Error();
#endif

