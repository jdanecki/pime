#ifndef DIALOG_H
#define DIALOG_H

#include <SDL2/SDL.h>
#include <list>
#include <string>

enum class DialogElementType
{
    Box,
    Text,
    Image,
    Button,
};

class DialogElement
{
  public:
    int id;
    enum DialogElementType c_id;
    DialogElement(int id, enum DialogElementType c_id);
    virtual void draw(SDL_Renderer * renderer);
    enum DialogElementType get_c_id();
};

class DialogBox : public DialogElement
{
  protected:
    bool fill;

  public:
    SDL_Color color;
    SDL_Rect rect;
    DialogBox(int id, SDL_Rect rect, SDL_Color color, bool fill);
    void draw(SDL_Renderer * renderer);
};

class DialogText : public DialogElement
{
  protected:
    int x;
    int y;
    int size;
    SDL_Color color;
    std::string text;

  public:
    DialogText(int id, int x, int y, int size, SDL_Color color, std::string text);
    void draw(SDL_Renderer * renderer);
};

class DialogImage : public DialogElement
{
  protected:
    SDL_Rect rect;

  public:
    SDL_Texture * texture;
    DialogImage(int id, SDL_Rect rect, std::string filename);
    DialogImage(int id, SDL_Rect rect);
    void draw(SDL_Renderer * renderer);
};

class DialogButton : public DialogElement
{
  public:
    DialogBox * d_box;
    DialogText * d_text;
    DialogButton(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int));
    DialogButton(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int), void (*on_secondary_press)(int));
    void draw(SDL_Renderer * renderer);
    void (*on_press)(int);
    void (*on_secondary_press)(int);
};

class Dialog
{
    std::list<DialogElement *> elements;
    SDL_Rect rect;
    SDL_Color background_color;

  public:
    Dialog(SDL_Rect rect, SDL_Color background_color);
    DialogElement * get_element_from_id(int id, enum DialogElementType c_id);
    void add_box(int id, SDL_Rect rect, SDL_Color color, bool fill);
    void add_text(int id, int x, int y, int size, SDL_Color color, std::string text);
    void add_image(int id, SDL_Rect rect, std::string filename);
    void add_image(int id, SDL_Rect rect);
    void add_button(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int));
    void add_button(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int), void (*on_secondary_press)(int));
    void draw(SDL_Renderer * renderer);
    void press(int x, int y, bool secondary);
    void press(int x, int y);
};

#endif
