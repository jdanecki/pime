#include "npc.h"
#include "menu.h"

extern Player * player;
Npc * current_npc;
extern int active_hotbar;

int npc(menu_actions a)
{
    if (menu_dialog)
    {
        delete menu_dialog;
    }
    SentencesList * list;
    if (a == MENU_NPC_SAY)
        list = sentences;
    else list = questions;

    int menu_entries = list->nr_elements;
    list->enable_all();

    if (a == MENU_NPC_ASK)
    {
        if (player->hotbar[active_hotbar])
        {
            list->enable(NPC_Ask_do_you_know_item);
        }
        else
        {
            list->disable(NPC_Ask_do_you_know_item);
            menu_entries--;
        }
    } else {
        Npc_say sid=NPC_Say_Nothing;
        Sentence * a=dynamic_cast<Sentence *>(sentences->find(&sid));
        a->disable();
        menu_entries--;
    }

    menu_dialog = new Menu("NPC conversation", menu_entries +1);

    Sentence * sentence = (Sentence *) list->head;
    while (sentence)
    {
        if (sentence->is_enabled())
        {
            if (sentence->id == NPC_Ask_do_you_know_item)
                menu_dialog->add(sentence->text, sentence->id, sentence, player->hotbar[active_hotbar]);
            else
                menu_dialog->add(sentence->text, sentence->id, sentence, nullptr);
        }
        sentence = (Sentence *)sentence->next;
    }
    menu_dialog->add("Cancel", MENU_CANCEL);
    current_menu = menu_dialog;   
    return 0;
}

int npc_say(Sentence * s)
{ //entry point for sentences and questions
    player->start_conversation(current_npc);
    if (s->id < NPC_Say_Nothing)
    {
        InventoryElement * el = menu_dialog->get_el();
        player->ask(s, el);
        return 0;
    }else {

            return player->say(s) ? 1 : 0;
    }
}
