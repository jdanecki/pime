#ifndef MENU_NPC_H
#define MENU_NPC_H
#include "../../core/alchemist/npc_talk.h"
#include "../../core/npc.h"
#include "../../2d/alchemist2d.h"
#include "menu.h"

int npc(menu_actions a);
int npc_say(Sentence * s, InventoryElement * el);
Menu * create_menu_npc();

#endif // NPC_H
