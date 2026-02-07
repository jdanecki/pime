
#include "playerUI.h"
#include "../core/world.h"

#include "npc.h"
#include "texture.h"

#include "../ui/menu/menu.h"


Npc2d * current_npc;


Npc2d::Npc2d(Npc data) : Npc(data)
{
}

Backend_Texture Npc2d::get_texture()
{
    texture_created = true;
    return Player_textures.npc;
}


