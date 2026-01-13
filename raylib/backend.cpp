#include <stdio.h>
#include "backend.inl"
#include <stdlib.h>

Texture2D Backend_Create_Texture_From_Surface(Image image)
{
    return LoadTextureFromImage(image);
}

const char * Backend_Get_Error()
{
    return "not implemented yet";
}

void Backend_Wait()
{
    //   WaitTime(0.2);
}

void Backend_Update_Screen()
{
    //    DrawFPS(900, 10);
}

void Backend_Texture_Copy_With_Mask(Texture2D texture, Backend_Rect * srcrect, Backend_Rect * dstrect, Color color, bool mask)
{
    Rectangle src;
    if (srcrect)
        src = srcrect->r;
    else
    {
        src.x = 0;
        src.y = 0;
        src.width = texture.width;
        src.height = texture.height;
    }
    if (mask)
        DrawTexturePro(texture, src, dstrect->r,
            {
                0,
            },
            0, color);
    else
        DrawTexturePro(texture, src, dstrect->r,
            {
                0,
            },
            0, WHITE);
}

void Backend_Texture_Copy(Texture2D texture, Backend_Rect * srcrect, Backend_Rect * dstrect)
{
    Rectangle src;
    if (srcrect)
        src = srcrect->r;
    else
    {
        src.x = 0;
        src.y = 0;
        src.width = texture.width;
        src.height = texture.height;
    }
    DrawTexturePro(texture, src, dstrect->r,
        {
            0,
        },
        0, WHITE);
}
void Backend_Texture_Copy_Flip(Texture2D texture, Backend_Rect * srcrect, Backend_Rect * dstrect)
{
    Rectangle src = srcrect->r;
    src.width = -src.width;
    DrawTexturePro(texture, srcrect->r, dstrect->r,
        {
            0,
        },
        0, WHITE);
}

int Backend_Get_Texture_Size(Texture texture, int * w, int * h)
{
    *w = texture.width;
    *h = texture.height;
    return 0;
}

Backend_Pixels Backend_Allocate_Pixels(int w, int h)
{
    Backend_Pixels pixels;
    Image img = GenImageColor(w, h, BLANK); // CPU, RGBA8888
    pixels.texture = LoadTextureFromImage(img);
    //    printf("Image.format=%d Texture.format=%d Texture.id=%d\n", img.format, pixels.texture.format, pixels.texture.id);
    UnloadImage(img);
    pixels.pixels = (unsigned char *)calloc(1, w * h * 4);
    pixels.pitch = w * 4;
    return pixels;
}

void Backend_Update_Texture_Pixels(Backend_Pixels pixels)
{
    UpdateTexture(pixels.texture, pixels.pixels);
}

void Backend_Draw_Fill_Rectangle(Backend_Rect r, Color color)
{
    DrawRectangleRec(r.r, color);
}

void Backend_Draw_Gradient_Rectangle(int x, int y, int w, int h, Color top, Color bottom)
{
    DrawRectangleGradientV(x, y, w, h, top, bottom);
}
void Backend_Draw_Rectangle(Backend_Rect r, Color color)
{
    DrawRectangleLinesEx(r.r, 1.0f, color);
}

void Backend_Window_Size(int * w, int * h)
{
    *w = GetScreenWidth();
    *h = GetScreenHeight();
}
