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

DialogBox::DialogBox(int id, int x, int y, int w, int h, SDL_Color color, bool fill) : DialogElement(id, DialogElementType::Box)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->color = color;
    this->fill = fill;
}

void DialogBox::draw(SDL_Renderer * renderer)
{
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, this->color.a);
    if (this->fill)
        SDL_RenderFillRect(renderer, &rect);
    else
        SDL_RenderDrawRect(renderer, &rect);
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

DialogImage::DialogImage(int id, int x, int y, int w, int h, std::string filename) : DialogElement(id, DialogElementType::Image)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->texture = load_texture(filename.c_str());
}

DialogImage::DialogImage(int id, int x, int y, int w, int h) : DialogElement(id, DialogElementType::Image)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->texture = NULL;
}

void DialogImage::draw(SDL_Renderer * renderer)
{
    if (this->texture == NULL)
        return;
    SDL_Rect rect = {this->x, this->y, this->w, this->h};
    SDL_RenderCopy(renderer, this->texture, NULL, &rect);
}

DialogButton::DialogButton(int id, int x, int y, int w, int h, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int)) : DialogElement(id, DialogElementType::Button)
{
    this->d_box = new DialogBox(id, x, y, w, h, bgcolor, 1);
    this->d_text = new DialogText(id, x, y, size, fgcolor, text);
    this->on_press = on_press;
}

void DialogButton::draw(SDL_Renderer * renderer)
{
    this->d_box->draw(renderer);
    this->d_text->draw(renderer);
}

Dialog::Dialog(int x, int y, int w, int h, SDL_Color background_color)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->background_color = background_color;
};

DialogElement * Dialog::get_element_from_id(int id, enum DialogElementType c_id)
{
    for (DialogElement * de : this->elements)
        if (de->id == id && de->c_id == c_id)
            return de;
    return NULL;
}

void Dialog::add_box(int id, int x, int y, int w, int h, SDL_Color color, bool fill)
{
    this->elements.push_back(new DialogBox(id, x + this->x, y + this->y, w, h, color, fill));
}

void Dialog::add_text(int id, int x, int y, int size, SDL_Color color, std::string text)
{
    this->elements.push_back(new DialogText(id, x + this->x, y + this->y, size, color, text));
}

void Dialog::add_image(int id, int x, int y, int w, int h, std::string filename)
{
    this->elements.push_back(new DialogImage(id, x + this->x, y + this->y, w, h, filename));
}

void Dialog::add_image(int id, int x, int y, int w, int h)
{
    this->elements.push_back(new DialogImage(id, x + this->x, y + this->y, w, h));
}

void Dialog::add_button(int id, int x, int y, int w, int h, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int))
{
    this->elements.push_back(new DialogButton(id, x + this->x, y + this->y, w, h, size, bgcolor, fgcolor, text, on_press));
}

void Dialog::draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, this->background_color.r, this->background_color.g, this->background_color.b, this->background_color.a);
    SDL_Rect rect = {this->x, this->y, this->w, this->h};
    SDL_RenderFillRect(renderer, &rect);
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
                if (box->x < x && box->y < y && box->x + box->w > x && box->y + box->h > y)
                {
                    button->on_press(button->id);
                }
            }
        }
    }
}
