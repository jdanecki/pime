#include "menu_inventory.h"
Menu * create_menu_inventory_categories()
{
    Menu * m = new Menu("Inventory categories");
    m->add("Elements", MENU_INV_ELEMENTS, Class_Element);
    m->add("Ingredients", MENU_INV_INGREDIENTS, Class_Ingredient);
    m->add("Products", MENU_INV_PRODUCT, Class_Product);
    m->add("Plants", MENU_INV_PLANT, Class_Plant);
    m->add("Animals", MENU_INV_ANIMAL, Class_Animal);
    m->add("Scrolls", MENU_INV_SCROLL, Class_Scroll);
    return m;
}
Menu * create_menu_inventory_elements()
{
    Menu * m = new Menu("Inventory elements");
    m->add("Solid form", MENU_INV_SOLID, Form_solid);
    m->add("Liquid form", MENU_INV_LIGQUID, Form_liquid);
    m->add("Gas form", MENU_INV_GAS, Form_gas);
    return m;
}