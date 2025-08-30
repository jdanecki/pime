#ifndef SDL_NPC_H
#define SDL_NPC_H
#include "../core/alchemist/npc_talk.h"
#include "../core/npc.h"
#include "implementations/alchemistSDL.h"
#include "menu.h"

extern int npc(menu_actions a);
extern int npc_say(Sentence * s, InventoryElement * el);

class NpcSDL : public Npc, public Renderable
{
    void init();

  public:
    NpcSDL(Npc data);
};

extern NpcSDL * current_npc;

#endif // NPC_H
