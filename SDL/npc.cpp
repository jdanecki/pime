#include "npc.h"
#include "menu.h"

extern Player * player;
Npc * current_npc;
extern int active_hotbar;

int npc()
{
    if (menu_dialog)
    {
        delete menu_dialog;
    }
   /* int menu_entries = sentences->nr_elements;

    sentences->enable_all();

    if (player->hotbar[active_hotbar])
    {
        questions->enable(NPC_Ask_do_you_know_item);
    }
    else
    {
        questions->disable(NPC_Ask_do_you_know_item);
        menu_entries--;
    }*/

    menu_dialog = new Menu("NPC", 3);

    menu_dialog->add("Cancel", MENU_CANCEL);

    /*Sentence * sentence = (Sentence *)sentences->head;
    while (sentence)
    {
        if (sentence->is_enabled())
        {
            if (sentence->id == NPC_Ask_do_you_know_item)
                menu_dialog->add("Do you know", MENU_NPC_SAY, sentence, player->hotbar[active_hotbar]);
            else
                menu_dialog->add(sentence->text, MENU_NPC_SAY, sentence, nullptr);
        }
        sentence = (Sentence *)sentence->next;
    }
*/
    current_menu = menu_dialog;
  //  player->start_conversation(current_npc);

    return 0;
}

int npc_say(Sentence * s)
{
    switch (s->id)
    {
        default:
            return player->say(s) ? 1 : 0;

        case NPC_Ask_do_you_know_item:
            InventoryElement * el = menu_dialog->get_el();
            player->ask(s, el);
            return 0;
    }
}
