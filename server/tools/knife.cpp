#include "tools.h"

Knife::Knife(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_KNIFE, Form_solid, 1)
{
    actions[0] = ACT_CUT;
}

ProductServer * Knife::createKnife(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_KNIFE_BLADE && id2 == ING_KNIFE_HANDLE) || (id1 == ING_KNIFE_HANDLE && id2 == ING_KNIFE_BLADE))
        return new Knife(el1, el2);
    CONSOLE_LOG(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return nullptr;
}

Knife * create_knife(chunk * ch)
{
    int count;
    NetworkObject ** base_solid = base_elements.find_form(Form_solid, &count);
    CONSOLE_LOG("count=%d \n", count);
    if (count)
    {
        for (int i = 0; i < count; i++)
        {
            BaseElement * base = static_cast<BaseElement *>(base_solid[i]);
            CONSOLE_LOG("solid: %d/%d\n", i, count);
            //            base->show(false);
        }

        ElementServer * el1 = create_element(static_cast<BaseElement *>(base_solid[0]));
        ElementServer * el2 = create_element(static_cast<BaseElement *>(base_solid[1]));
        IngredientServer * kb = KnifeBlade::createKnifeBlade(el1);
        IngredientServer * kh = KnifeHandle::createKnifeHandle(el2);
        ch->add_object(kb, 0, 0);
        ch->add_object(kh, 0, 0);
        Knife * knife = static_cast<Knife *>(Knife::createKnife(kb, kh));
        destroy(kb);
        destroy(kh);
        return knife;
    }
    return nullptr;
}
