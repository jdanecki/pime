#include "craft_ing.h"

InventoryElement * craft_ing(int product_id, InventoryElement * el)
{
    InventoryElement * crafted=nullptr;

    switch(product_id)
    {
        case ING_AXE_BLADE: crafted  = new AxeBlade(el); break;
        case ING_AXE_HANDLE: crafted = new AxeHandle(el); break;
        case ING_PICKAXE_BLADE: crafted  = new PickAxeBlade(el); break;
        case ING_PICKAXE_HANDLE: crafted = new PickAxeHandle(el); break;
        case ING_WALL: crafted = new Wall(el); break;
        case ING_KNIFE_BLADE: crafted  = new KnifeBlade(el); break;
        case ING_KNIFE_HANDLE: crafted = new KnifeHandle(el); break;

    }

    if (crafted->craft())
    {
        crafted->show();
        return crafted;
    }
    else
    {
        delete crafted;
    }


    return nullptr;
}
