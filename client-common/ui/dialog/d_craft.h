#ifndef D_CRAFT_H
#define D_CRAFT_H

#include "dialog.h"
#include "../../2d/texture.h"
#include "../../../core/alchemist/el_list.h"

class DElements : public Dialog
{
	void add_elems();
	void (*button1_press)(DialogButton *);
	void (*button2_press)(DialogButton *);
public:
	ElementsList *el_list;
	DElements(Backend_Rect r, int count, Backend_Texture * textures, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *) = nullptr);
	DElements(Backend_Rect r, ElementsList *list, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *) = nullptr);
	void update();
};

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

extern DCraft *d_craft;

bool hide_craft_window();
void create_dialogs();

#endif
