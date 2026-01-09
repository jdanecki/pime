#ifndef D_HOTBAR_H
#define D_HOTBAR_H

#include "dialog.h"
class DHotbar : public Dialog
{
  public:
    DHotbar();
    void update();
};

extern int hotbar_x, hotbar_y;
#endif
