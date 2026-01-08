#ifndef D_CRAFT_H
#define D_CRAFT_H

#include "dialog.h"
class DCraft : public Dialog
{
  public:    
    Dialog ingredients;
    Dialog products;
    bool in_products;
    bool show;
    DCraft();
    void update();
    void draw();
    bool press(int x, int y, int button);

};

extern DCraft d_craft;

#endif
