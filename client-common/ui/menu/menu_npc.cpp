#include "playerUI.h"
#include "../../core/world.h"

#include "menu_npc.h"

extern int active_hotbar;
Menu * create_menu_npc()
{
    Menu * m = new Menu("NPC menu");
    m->add("Talk to NPC", MENU_NPC_SAY);
    m->add("Ask NPC", MENU_NPC_ASK);
    return m;
}

int npc(menu_actions a)
{
    if (menu_dialog)
    {
        delete menu_dialog;
    }
    SentencesList * list;
    if (a == MENU_NPC_SAY)
        list = sentences;
    else
        list = questions;

    list->enable_all();

    if (a == MENU_NPC_ASK)
    {
        if (player->hotbar[active_hotbar])
        {
            list->enable(NPC_Ask_do_you_know_inv_item);
        }
        else
        {
            list->disable(NPC_Ask_do_you_know_inv_item);
        }
    }
    else
    {
        Npc_say sid = NPC_Say_Nothing;
        Sentence * a = dynamic_cast<Sentence *>(sentences->find(&sid));
        a->disable();
    }

    menu_dialog = new Menu("NPC conversation");

    Sentence * sentence = (Sentence *)list->head;
    while (sentence)
    {
        if (sentence->is_enabled())
        {
            switch (sentence->id)
            {
                case NPC_Ask_do_you_know_inv_item:
                    menu_dialog->add(sentence->text, sentence->id, player->hotbar[active_hotbar], sentence);
                    break;
                default:
                    menu_dialog->add(sentence->text, sentence->id, nullptr, sentence);
                    break;
            }
        }
        sentence = (Sentence *)sentence->next;
    }
    menu_dialog->add("NPC menu", MENU_NPC);
    current_menu = menu_dialog;
    return 0;
}
