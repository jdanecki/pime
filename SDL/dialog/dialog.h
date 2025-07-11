#ifndef DIALOG_HPP
#define DIALOG_HPP

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
    int x;
    int y;
    int w;
    int h;
    DialogBox(int id, int x, int y, int w, int h, SDL_Color color, bool fill);
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
    int x;
    int y;
    int w;
    int h;

  public:
    SDL_Texture * texture;
    DialogImage(int id, int x, int y, int w, int h, std::string filename);
    DialogImage(int id, int x, int y, int w, int h);
    void draw(SDL_Renderer * renderer);
};

class DialogButton : public DialogElement
{
  public:
    DialogBox * d_box;
    DialogText * d_text;
    DialogButton(int id, int x, int y, int w, int h, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int));
    void draw(SDL_Renderer * renderer);
    void (*on_press)(int);
};

class Dialog
{
    std::list<DialogElement *> elements;
    int x;
    int y;
    int w;
    int h;
    SDL_Color background_color;

  public:
    Dialog(int x, int y, int w, int h, SDL_Color background_color);
    DialogElement * get_element_from_id(int id, enum DialogElementType c_id);
    void add_box(int id, int x, int y, int w, int h, SDL_Color color, bool fill);
    void add_text(int id, int x, int y, int size, SDL_Color color, std::string text);
    void add_image(int id, int x, int y, int w, int h, std::string filename);
    void add_image(int id, int x, int y, int w, int h);
    void add_button(int id, int x, int y, int w, int h, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(int));
    void draw(SDL_Renderer * renderer);
    void press(int x, int y);
};

#endif
