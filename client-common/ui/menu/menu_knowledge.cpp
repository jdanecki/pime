#include "menu_knowledge.h"

Menu * create_menu_knowledge()
{
    Menu * m = new Menu("Knowledge");
    m->add("Elements", MENU_KNOWLEDGE_ELEMENTS, Class_BaseElement);
    m->add("Plants", MENU_KNOWLEDGE_PLANTS, Class_BasePlant);
    m->add("Animals", MENU_KNOWLEDGE_ANIMALS, Class_BaseAnimal);
    return m;
}
