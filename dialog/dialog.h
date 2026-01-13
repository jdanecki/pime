#ifndef DIALOG_H
#define DIALOG_H

#include <list>
#include <string>

#include "backend.inl"

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
  public:
    Backend_Rect rect;

    Dialog *dialog;
    int id;
    enum DialogElementType c_id;
    DialogElement(int id, Backend_Rect rect, enum DialogElementType c_id);
    virtual void draw();
    virtual bool pressed(int x, int y)
    {
        return false;
    }
    enum DialogElementType get_c_id();
    virtual void move(int x, int y)
    {
        rect.r.x += x;
        rect.r.y += y;
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
    Backend_Color background_color;

  public:
    Dialog(Backend_Rect rect, Backend_Color background_color);
    DialogElement * get_element_from_id(int id, enum DialogElementType c_id);
    void add(DialogElement * el);
    void draw();
    bool press(int x, int y, int button);
};

class DialogBox : public DialogElement
{
  public:
    Backend_Color color;
    bool fill;
    DialogBox(int id, Backend_Rect rect, Backend_Color color, bool fill);
    void draw();
};

class DialogText : public DialogElement
{

  protected:
    int size;
    Backend_Color color;
    std::string text;

  public:
    DialogText(int id, int x, int y, int size, Backend_Color color, std::string text);
    void draw();
};

class DialogImage : public DialogElement
{
  public:
    bool texture_loaded;
    Backend_Texture texture;
    DialogImage(int id, Backend_Rect rect, std::string filename);
    DialogImage(int id, Backend_Rect rect);
    void draw();
};

class DialogButton : public DialogElement
{

  public:
    DialogBox * d_box;
    DialogText * d_text;
    DialogButton(int id, Backend_Rect rect, int size, Backend_Color bgcolor, Backend_Color fgcolor, std::string text, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *) = nullptr);
    void draw();
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
