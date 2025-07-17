#ifndef D_CRAFT_H
#define D_CRAFT_H

#include "dialog.h"
#include <SDL2/SDL_render.h>
class DCraft : public Dialog
{
  public:
    Dialog ingredients;
    Dialog products;
    bool in_products;
    bool show;
    DCraft();
    void update();
    void draw(SDL_Renderer * renderer);
    void press(int x, int y);
};

extern DCraft d_craft;

#endif
