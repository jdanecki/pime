#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H

typedef void (*KeyHandlerFunc)();

struct KeyHandler {
    int key;
    KeyHandlerFunc func;
    bool press_key;
};

#endif // KEY_HANDLER_H
