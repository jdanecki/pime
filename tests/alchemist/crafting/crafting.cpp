#include "test_axe.h"
#include "test_knife.h"

#include "../../server/cpp-src/elements_server.h"

void test()
{
    printf("%sa - test axe\n", colorCyan);
    printf("%sk - test knife\n", colorCyan);
    printf("%s%s", colorNormal, colorGreenBold);

    char c = wait_key('t');
    InventoryElement * el = nullptr;
    switch (c)
    {
        case 'a':
            el = test_axe();
            break;
        case 'k':
            el = test_knife();
            break;
    }
    if (el)
        player->inventory->add(el);
}

InventoryElement * craft2_ing(char c)
{
    InventoryElement * target = nullptr;
    InventoryElement * el2[2];
    el2[0] = nullptr;
    el2[1] = nullptr;
    if (!select_inventory2(2, el2))
        return nullptr;
    switch (c)
    {
        case 'a':
            target = Axe::createAxe(el2[0], el2[1]);
            break;
        case '1':
            target = Pickaxe::createPickaxe(el2[0], el2[1]);
            break;
        case '4':
            target = Knife::createKnife(el2[0], el2[1]);
            break;
        case '7':
            target = Hut::createHut(el2[0], el2[1]);
            break;
    }
    if (!target)
        return nullptr;

    player->inventory->remove(el2[0]);
    player->inventory->remove(el2[1]);
    return target;
}

InventoryElement * craft_ing(char c)
{
    InventoryElement * target = nullptr;
    InventoryElement * el = select_element(player->inventory);
    if (!el)
        return nullptr;

    switch (c)
    {
        case 'b':
            target = AxeBlade::createAxeBlade(el);
            break;
        case 'h':
            target = AxeHandle::createAxeHandle(el);
            break;
        case '2':
            target = PickaxeBlade::createPickaxeBlade(el);
            break;
        case '3':
            target = PickaxeHandle::createPickaxeHandle(el);
            break;
        case '5':
            target = KnifeBlade::createKnifeBlade(el);
            break;
        case '6':
            target = KnifeHandle::createKnifeHandle(el);
            break;

        case 'w':
            target = Wall::createWall(el);
            break;
    }
    if (!target)
        return nullptr;
    player->inventory->remove(el);
    return target;
}

void craft_entry()
{
    printf("%sa - craft axe\n", colorCyan);
    printf("b - craft axe blade\n");
    printf("h - craft axe handle\n");

    printf("1 - craft pickaxe\n");
    printf("2 - craft pickaxe blade\n");
    printf("3 - craft pickaxe handle\n");

    printf("4 - craft knife\n");
    printf("5 - craft knife blade\n");
    printf("6 - craft knife handle\n");

    printf("7 - craft hut\n");
    printf("w - craft wall\n");

    printf("%s%s", colorNormal, colorGreenBold);

    InventoryElement * target = nullptr;
    char c = wait_key('c');
    switch (c)
    {
        case 'a':
        case '1':
        case '4':
        case '7':
            target = craft2_ing(c);
            break;
        case 'b':
        case 'h':
        case 'w':
        case '2':
        case '3':
        case '5':
        case '6':
            target = craft_ing(c);
            break;
    }
    if (target)
    {
        player->inventory->add(target);
        player->set_known(target->get_base_cid(), target->get_id());
        printf("%s added to inventory\n", target->get_name());
    }
}


