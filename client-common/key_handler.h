#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H

typedef void (*KeyHandlerFunc)();

struct KeyHandler {
    int key;
    KeyHandlerFunc func;
};

#endif // KEY_HANDLER_H
