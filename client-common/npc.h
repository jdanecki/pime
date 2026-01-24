#ifndef SDL_NPC_H
#define SDL_NPC_H
#include "../core/alchemist/npc_talk.h"
#include "../core/npc.h"
#include "alchemist2d.h"
#include "../menu/menu.h"

extern int npc(menu_actions a);
extern int npc_say(Sentence * s, InventoryElement * el);

class Npc2d : public Npc, public Renderable
{
    void init();

  public:
    Npc2d(Npc data);
    Backend_Texture get_texture() override;
};

extern Npc2d * current_npc;

#endif // NPC_H
