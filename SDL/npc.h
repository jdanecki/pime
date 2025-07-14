#ifndef SDL_NPC_H
#define SDL_NPC_H
#include "../core/alchemist/npc_talk.h"
#include "../core/npc.h"
#include "menu.h"

extern int npc(menu_actions a);
extern int npc_say(Sentence * s, InventoryElement *el);
extern Npc * current_npc;
#endif // NPC_H
