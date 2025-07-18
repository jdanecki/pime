#include "plants.h"
#include "../player.h"
#include "show_list.h"

extern Player * player;
extern InvList * elements;
InvList * plants;

void sow_plant()
{
    ListElement * cur = player->inventory->head;
    while (cur)
    {
        if (cur->el->c_id != Class_Plant)
            cur->disable();
        cur = cur->next;
    }

    InventoryElement * el = select_element(player->inventory);
    player->inventory->enable_all();
    if (!el)
        return;
    player->inventory->remove(el);
    plants->add(el);
    Plant * p = (Plant *)el;
    p->planted = 1;

    printf("%s planted\n", el->get_name());
}

void harvest_plant()
{
    Plant * el = dynamic_cast<Plant *>(select_element(plants));
    //   plants->enable_all();
    if (!el)
        return;
    // FIXME
    // el->change_phase(Plant_seed);
    player->inventory->add(el);
    plants->remove(el);
    printf("plant: %s harvested to inventory\n", el->get_name());
}
