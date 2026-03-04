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
    bool visible;
    Dialog * dialog;
    int id;
    enum DialogElementType c_id;
    DialogElement(int id, Backend_Rect rect, enum DialogElementType c_id);
    ~DialogElement() {}
    virtual void draw();
    virtual bool pressed(int x, int y);
    enum DialogElementType get_c_id();
    virtual void move(int x, int y);
    bool in_rect(int x, int y);
    bool check_id(int id, enum DialogElementType c_id);
    virtual Dialog * get_dialog();

};

class Dialog : public DialogElement
{
    std::list<DialogElement *> elements;
    Backend_Color background_color;
    DialogElement * get_element_from_id(int id, enum DialogElementType c_id);
  public:
    Dialog(Backend_Rect rect, Backend_Color background_color);
    ~Dialog() {}
    template<typename T>
    T * get(int id);

    void add(DialogElement * el);
    void draw();
    bool press(int x, int y, int button);
    static constexpr DialogElementType t = DialogElementType::Dialog;
};

template<typename T>
T * Dialog::get(int id)
{
	return static_cast<T*>(get_element_from_id(id, T::t));
}

class DialogBox : public DialogElement
{
  public:
    Backend_Color color;
    bool fill;
    DialogBox(int id, Backend_Rect rect, Backend_Color color, bool fill);
    ~DialogBox(){}
    void draw();
    static constexpr DialogElementType t = DialogElementType::Box;
};

class DialogText : public DialogElement
{

  protected:
    int size;
    Backend_Color color;
    std::string text;

  public:
    DialogText(int id, int x, int y, int size, Backend_Color color, std::string text);
    ~DialogText() {}
    void draw();
    void change_text(std::string text);
    static constexpr DialogElementType t = DialogElementType::Text;
};

class DialogImage : public DialogElement
{
  public:
    bool texture_loaded;
    Backend_Texture texture;
    DialogImage(int id, Backend_Rect rect, std::string filename);
    DialogImage(int id, Backend_Rect rect);
    ~DialogImage() {}
    void draw();
    static constexpr DialogElementType t = DialogElementType::Image;
};

class DialogButton : public DialogElement
{

  public:
    DialogBox * d_box;
    DialogText * d_text;
    DialogButton(
        int id, Backend_Rect rect, int size, Backend_Color bgcolor, Backend_Color fgcolor, std::string text, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *) = nullptr);
    ~DialogButton() {}
    void draw();
    bool pressed(int x, int y);
    void (*on_press)(DialogButton *);
    void (*on_secondary_press)(DialogButton *);
    void move(int x, int y);
    static constexpr DialogElementType t = DialogElementType::Button;
};

#endif
