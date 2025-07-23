#include "dialog.h"
#include "../text.h"
#include "../texture.h"
#include <SDL2/SDL_render.h>

DialogElement::DialogElement(int id, enum DialogElementType c_id)
{
    this->id = id;
    this->c_id = c_id;
};

void DialogElement::draw(SDL_Renderer * renderer)
{
}

enum DialogElementType DialogElement::get_c_id()
{
    return this->c_id;
}

DialogBox::DialogBox(int id, SDL_Rect rect, SDL_Color color, bool fill) : DialogElement(id, DialogElementType::Box)
{
    this->rect = rect;
    this->color = color;
    this->fill = fill;
}

void DialogBox::draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, this->color.a);
    if (this->fill)
        SDL_RenderFillRect(renderer, &this->rect);
    else
        SDL_RenderDrawRect(renderer, &this->rect);
}

DialogText::DialogText(int id, int x, int y, int size, SDL_Color color, std::string text) : DialogElement(id, DialogElementType::Text)
{
    this->x = x;
    this->y = y;
    this->size = size;
    this->color = color;
    this->text = text;
}

void DialogText::draw(SDL_Renderer * renderer)
{
    write_text(this->x, this->y, this->text.c_str(), this->color, this->size, this->size * 1.5);
}

DialogImage::DialogImage(int id, SDL_Rect rect, std::string filename) : DialogElement(id, DialogElementType::Image)
{
    this->rect = rect;
    this->texture = load_texture(filename.c_str());
}

DialogImage::DialogImage(int id, SDL_Rect rect) : DialogElement(id, DialogElementType::Image)
{
    this->rect = rect;
    this->texture = NULL;
}

void DialogImage::draw(SDL_Renderer * renderer)
{
    if (this->texture == NULL)
        return;
    SDL_RenderCopy(renderer, this->texture, NULL, &this->rect);
}

DialogButton::DialogButton(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int)) : DialogElement(id, DialogElementType::Button)
{
    this->d_box = new DialogBox(id, rect, bgcolor, 1);
    this->d_text = new DialogText(id, rect.x, rect.y, size, fgcolor, text);
    this->on_press = on_press;
}

void DialogButton::draw(SDL_Renderer * renderer)
{
    this->d_box->draw(renderer);
    this->d_text->draw(renderer);
}

Dialog::Dialog(SDL_Rect rect, SDL_Color background_color)
{
    this->rect = rect;
    this->background_color = background_color;
};

DialogElement * Dialog::get_element_from_id(int id, enum DialogElementType c_id)
{
    for (DialogElement * de : this->elements)
        if (de->id == id && de->c_id == c_id)
            return de;
    return NULL;
}

void offset_rect(SDL_Rect * rect, int x, int y)
{
    rect->x += x;
    rect->y += y;
}

void offset_rect(SDL_Rect * rect1, SDL_Rect * rect2)
{
    offset_rect(rect1, rect2->x, rect2->y);
}

void Dialog::add_box(int id, SDL_Rect rect, SDL_Color color, bool fill)
{
    offset_rect(&rect, &this->rect);
    this->elements.push_back(new DialogBox(id, rect, color, fill));
}

void Dialog::add_text(int id, int x, int y, int size, SDL_Color color, std::string text)
{
    this->elements.push_back(new DialogText(id, x + this->rect.x, y + this->rect.y, size, color, text));
}

void Dialog::add_image(int id, SDL_Rect rect, std::string filename)
{
    offset_rect(&rect, &this->rect);
    this->elements.push_back(new DialogImage(id, rect, filename));
}

void Dialog::add_image(int id, SDL_Rect rect)
{
    offset_rect(&rect, &this->rect);
    this->elements.push_back(new DialogImage(id, rect));
}

void Dialog::add_button(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int))
{
    offset_rect(&rect, &this->rect);
    this->elements.push_back(new DialogButton(id, rect, size, bgcolor, fgcolor, text, on_press));
}

void Dialog::draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, this->background_color.r, this->background_color.g, this->background_color.b, this->background_color.a);
    SDL_RenderFillRect(renderer, &this->rect);
    for (DialogElement * de : this->elements)
    {
        de->draw(renderer);
    }
}

void Dialog::press(int x, int y)
{
    for (DialogElement * de : this->elements)
    {
        if (de->get_c_id() == DialogElementType::Button)
        {
            DialogButton * button = dynamic_cast<DialogButton *>(de);
            if (button->on_press)
            {
                DialogBox * box = dynamic_cast<DialogBox *>(button->d_box);
                if (box->rect.x < x && box->rect.y < y && box->rect.x + box->rect.w > x && box->rect.y + box->rect.h > y)
                {
                    button->on_press(button->id);
                }
            }
        }
    }
}
