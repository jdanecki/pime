#include "d_action.h"
#include "../main.h"
#include "dialog.h"

void change_active_action(DialogButton * button)
{
    printf("action id=%d\n", button->id);
}

DAction::DAction(int count) : Dialog({650, 540, 320, 32}, {0, 0, 0, 0})
{
    for (int i = 0; i < count; i++)
    {
        add(new DialogBox(i, {i * 32, 0, 32, 32}, {125, 125, 125, 255}, false));
        add(new DialogImage(i, {i * 32 + 2, 2, 32 - 4, 32 - 4}));
        //dialog->add(new DialogImage(0, {150, 23, 75, 75}, "textures/npc.png"));
        add(new DialogButton(i, {i * 32 + 2, 2, 32 - 4, 32 - 4}, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, "", change_active_action, nullptr));
    }
}
