#include "dialog.h"
#include "../../2d/texture.h"
#include "../text.h"

DialogElement::DialogElement(int id, Backend_Rect rect, enum DialogElementType c_id) : rect(rect), id(id), c_id(c_id)
{
    dialog = nullptr;
    visible = true;
}

void DialogElement::draw()
{
}

enum DialogElementType DialogElement::get_c_id()
{
    return c_id;
}

bool DialogElement::in_rect(int x, int y)
{
    return rect.r.x < x && rect.r.y < y && rect.r.x + rect.get_w() > x && rect.r.y + rect.get_h() > y;
}

DialogBox::DialogBox(int id, Backend_Rect rect, Backend_Color color, bool fill) :
		DialogElement(id, rect, t), color(color), fill(fill)
{
}

void DialogBox::draw()
{
	if (!visible) return;
    if (fill) {
    	Backend_Draw_Fill_Rectangle(rect, color);
        Backend_Line(rect.r.x, rect.r.y, rect.r.x + rect.get_w()-1, rect.r.y, (Backend_Color){0, 0, 0, 255});
        Backend_Line(rect.r.x, rect.r.y, rect.r.x, rect.r.y+rect.get_h()-1, (Backend_Color){0, 0, 0, 255});

        Backend_Line(rect.r.x + rect.get_w()-1, rect.r.y, rect.r.x + rect.get_w()-1, rect.r.y+rect.get_h()-1, (Backend_Color){255, 255, 255, 255});
        Backend_Line(rect.r.x, rect.r.y + rect.get_h()-1, rect.r.x + rect.get_w()-1, rect.r.y+rect.get_h()-1, (Backend_Color){255, 255, 255, 255});
    }
    else
        Backend_Draw_Rectangle(rect, color);
}

DialogText::DialogText(int id, int x, int y, int size, Backend_Color color, std::string text)
    : DialogElement(id, Backend_Rect(x, y, 0, 0), t), size(size), color(color), text(text)
{
}

void DialogText::draw()
{
	if (!visible) return;
	if (!text.size()) return;
    write_text(rect.r.x, rect.r.y, text.c_str(), color, size, size * 1.5);
}

void DialogText::change_text(std::string text)
{
    this->text = text;
}

DialogImage::DialogImage(int id, Backend_Rect rect, std::string filename) : DialogElement(id, rect, t)
{
    set_texture(load_texture(filename.c_str()));
}

DialogImage::DialogImage(int id, Backend_Rect rect) : DialogElement(id, rect, t)
{
	visible = false;
}

void DialogImage::set_texture(Backend_Texture texture)
{
	this->texture = texture;
	visible = true;
}
void DialogImage::draw()
{
	if (!visible) return;
        Backend_Texture_Copy(texture, nullptr, &rect);
}

DialogButton::DialogButton(
    int id, Backend_Rect rect, int size, Backend_Color bgcolor, Backend_Color fgcolor, std::string text,
	void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *))
    : DialogElement(id, rect, t), on_press(on_press), on_secondary_press(on_secondary_press)
{
    d_box = new DialogBox(id, rect, bgcolor, 1);
    d_text = new DialogText(id, rect.r.x+1, rect.r.y+1, size, fgcolor, text);
    d_image = nullptr;
}

DialogButton::DialogButton(
    int id, Backend_Rect rect, int size, Backend_Color bgcolor, Backend_Color fgcolor, Backend_Texture texture,
	void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *))
    : DialogElement(id, rect, t), on_press(on_press), on_secondary_press(on_secondary_press)
{
    d_box = new DialogBox(id, rect, bgcolor, 1);
    d_text = nullptr;
    d_image = new DialogImage(id, Backend_Rect(rect.r.x + 2, rect.r.y + 2, rect.get_w()-4, rect.get_h()-4));
    d_image->set_texture(texture);
}

void DialogButton::draw()
{
	if (!visible) return;
    d_box->draw();
    if (d_text) d_text->draw();
    if (d_image) d_image->draw();
}

bool DialogButton::pressed(int x, int y)
{
    return d_box->in_rect(x, y);
}

Dialog::Dialog(Backend_Rect rect, Backend_Color background_color) : DialogElement(0, rect, t), background_color(background_color)
{
}

DialogElement * Dialog::get_element_from_id(int id, enum DialogElementType c_id)
{
    for (DialogElement * de : elements)
    {
        if (de->check_id(id, c_id))
            return de;
    }
    return nullptr;
}

void Dialog::clear_elements()
{
	elements.clear();
}
void Dialog::add(DialogElement * el)
{
    elements.push_back(el);
    el->move(rect.r.x, rect.r.y);
    el->dialog = this;
}

void Dialog::draw()
{
	if (!visible) return;
    Backend_Draw_Fill_Rectangle(rect, background_color);
    for (DialogElement * de : elements)
    {
        de->draw();
    }
}

bool Dialog::press(int x, int y, int button)
{
    for (DialogElement * de : elements)
    {
        if (de->pressed(x, y))
        {
            DialogButton * d_button = static_cast<DialogButton *>(de);
            switch (button)
            {
                case 1:
                    if (d_button->on_press)
                        d_button->on_press(d_button);
                    break;
                case 3:
                    if (d_button->on_secondary_press)
                        d_button->on_secondary_press(d_button);
            }
            return true;
        }
    }
    return false;
}
bool DialogElement::check_id(int id, enum DialogElementType c_id)
{
    return this->id == id && this->c_id == c_id;
}
void DialogElement::move(int x, int y)
{
    rect.r.x += x;
    rect.r.y += y;
}
Dialog * DialogElement::get_dialog()
{
    return dialog;
}
bool DialogElement::pressed(int x, int y)
{
    return false;
}
void DialogButton::move(int x, int y)
{
    d_box->move(x, y);
    if (d_text) d_text->move(x, y);
    if (d_image) d_image->move(x, y);
}
