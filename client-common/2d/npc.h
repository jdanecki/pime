#ifndef CLIENT_COMMON_NPC_H
#define CLIENT_COMMON_NPC_H
#include "../core/npc.h"
#include "alchemist2d.h"
#include "backend.inl"

class Npc2d : public Npc, public Renderable
{

  public:
    Npc2d(Npc data);
    Backend_Texture get_texture() override;
};

extern Npc2d * current_npc;

#endif // NPC_H
