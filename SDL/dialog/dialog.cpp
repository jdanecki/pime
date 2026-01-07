#include <SDL2/SDL_render.h>

#include "dialog.h"
#include "../client-common/text.h"
#include "../client-common/texture.h"

DialogElement::DialogElement(int id, SDL_Rect rect, enum DialogElementType c_id) : rect(rect), id(id), c_id(c_id)
{
    dialog = nullptr;
}

void DialogElement::draw(SDL_Renderer * renderer)
{
}

enum DialogElementType DialogElement::get_c_id()
{
    return c_id;
}

bool DialogElement::in_rect(int x, int y)
{
    return rect.x < x && rect.y < y && rect.x + rect.w > x && rect.y + rect.h > y;
}

DialogBox::DialogBox(int id, SDL_Rect rect, SDL_Color color, bool fill) : DialogElement(id, rect, DialogElementType::Box), fill(fill), color(color)
{
}

void DialogBox::draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (fill)
        SDL_RenderFillRect(renderer, &rect);
    else
        SDL_RenderDrawRect(renderer, &rect);

   // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
   // SDL_RenderDrawRect(renderer, &rect);
}

DialogText::DialogText(int id, int x, int y, int size, SDL_Color color, std::string text) : DialogElement(id, {x, y, 0, 0}, DialogElementType::Text), size(size), color(color), text(text)
{
}

void DialogText::draw(SDL_Renderer *)
{
    write_text(rect.x, rect.y, text.c_str(), color, size, size * 1.5);
}

DialogImage::DialogImage(int id, SDL_Rect rect, std::string filename) : DialogElement(id, rect, DialogElementType::Image)
{
    texture = load_texture(filename.c_str());
}

DialogImage::DialogImage(int id, SDL_Rect rect) : DialogElement(id, rect, DialogElementType::Image)
{
    texture = nullptr;
}

void DialogImage::draw(SDL_Renderer * renderer)
{
    if (texture == nullptr)
        return;
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

DialogButton::DialogButton(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *))
    : DialogElement(id, rect, DialogElementType::Button), on_press(on_press), on_secondary_press(on_secondary_press)
{
    d_box = new DialogBox(id, rect, bgcolor, 1);
    d_text = new DialogText(id, rect.x, rect.y, size, fgcolor, text);
}

void DialogButton::draw(SDL_Renderer * renderer)
{
    d_box->draw(renderer);
    d_text->draw(renderer);
}

bool DialogButton::pressed(int x, int y)
{
    return d_box->in_rect(x, y);
}

Dialog::Dialog(SDL_Rect rect, SDL_Color background_color) : DialogElement(0, rect, DialogElementType::Dialog), background_color(background_color)
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
    el->move(rect.x, rect.y);
    el->dialog = this;
}

void Dialog::draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderFillRect(renderer, &rect);
    for (DialogElement * de : elements)
    {
        de->draw(renderer);
    }
}

bool Dialog::press(int x, int y, int button)
{
    for (DialogElement * de : elements)
    {
        if (de->pressed(x, y))
        {
            DialogButton * d_button = dynamic_cast<DialogButton *>(de);
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
