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
    Dialog,
};

class Dialog;
class DialogElement
{
  protected:
    SDL_Rect rect;

  public:
    Dialog *dialog;
    int id;
    enum DialogElementType c_id;
    DialogElement(int id, SDL_Rect rect, enum DialogElementType c_id);
    virtual void draw(SDL_Renderer * renderer);
    virtual bool pressed(int x, int y)
    {
        return false;
    }
    enum DialogElementType get_c_id();
    virtual void move(int x, int y)
    {
        rect.x += x;
        rect.y += y;
    }
    bool in_rect(int x, int y);
    bool check_id(int i, enum DialogElementType c)
    {
        return id == i && c_id == c;
    }
    virtual Dialog * get_dialog() {return dialog; }
};

class Dialog : public DialogElement
{
    std::list<DialogElement *> elements;
    SDL_Color background_color;

  public:
    Dialog(SDL_Rect rect, SDL_Color background_color);
    DialogElement * get_element_from_id(int id, enum DialogElementType c_id);
    void add(DialogElement * el);
    void draw(SDL_Renderer * renderer);
    bool press(int x, int y, int button);
};

class DialogBox : public DialogElement
{
  public:
    SDL_Color color;
    bool fill;
    DialogBox(int id, SDL_Rect rect, SDL_Color color, bool fill);
    void draw(SDL_Renderer * renderer);
};

class DialogText : public DialogElement
{

  protected:
    int size;
    SDL_Color color;
    std::string text;

  public:
    DialogText(int id, int x, int y, int size, SDL_Color color, std::string text);
    void draw(SDL_Renderer *);
};

class DialogImage : public DialogElement
{

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
    DialogButton(int id, SDL_Rect rect, int size, SDL_Color bgcolor, SDL_Color fgcolor, std::string text, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *) = nullptr);
    void draw(SDL_Renderer * renderer);
    bool pressed(int x, int y);
    void (*on_press)(DialogButton *);
    void (*on_secondary_press)(DialogButton *);
    void move(int x, int y)
    {
        d_box->move(x, y);
        d_text->move(x, y);
    }
};


#endif
