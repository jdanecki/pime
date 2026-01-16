#include "dialog.h"
#include "../client-common/text.h"
#include "../client-common/texture.h"

DialogElement::DialogElement(int id, Backend_Rect rect, enum DialogElementType c_id) : rect(rect), id(id), c_id(c_id)
{
    dialog = nullptr;
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

DialogBox::DialogBox(int id, Backend_Rect rect, Backend_Color color, bool fill) : DialogElement(id, rect, DialogElementType::Box), color(color), fill(fill)
{
}

void DialogBox::draw()
{
    if (fill)
        Backend_Draw_Fill_Rectangle(rect, color);
    else
        Backend_Draw_Rectangle(rect, color);
}

DialogText::DialogText(int id, int x, int y, int size, Backend_Color color, std::string text)
    : DialogElement(id, Backend_Rect(x, y, 0, 0), DialogElementType::Text), size(size), color(color), text(text)
{
}

void DialogText::draw()
{
    write_text(rect.r.x, rect.r.y, text.c_str(), color, size, size * 1.5);
}

DialogImage::DialogImage(int id, Backend_Rect rect, std::string filename) : DialogElement(id, rect, DialogElementType::Image)
{
    texture = load_texture(filename.c_str());
    texture_loaded = true;
}

DialogImage::DialogImage(int id, Backend_Rect rect) : DialogElement(id, rect, DialogElementType::Image)
{
    texture_loaded = false;
}

void DialogImage::draw()
{
    if (texture_loaded)
        Backend_Texture_Copy(texture, nullptr, &rect);
}

DialogButton::DialogButton(
    int id, Backend_Rect rect, int size, Backend_Color bgcolor, Backend_Color fgcolor, std::string text, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *))
    : DialogElement(id, rect, DialogElementType::Button), on_press(on_press), on_secondary_press(on_secondary_press)
{
    d_box = new DialogBox(id, rect, bgcolor, 1);
    d_text = new DialogText(id, rect.r.x, rect.r.y, size, fgcolor, text);
}

void DialogButton::draw()
{
    d_box->draw();
    d_text->draw();
}

bool DialogButton::pressed(int x, int y)
{
    return d_box->in_rect(x, y);
}

Dialog::Dialog(Backend_Rect rect, Backend_Color background_color) : DialogElement(0, rect, DialogElementType::Dialog), background_color(background_color)
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

void Dialog::add(DialogElement * el)
{
    elements.push_back(el);
    el->move(rect.r.x, rect.r.y);
    el->dialog = this;
}

void Dialog::draw()
{
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
