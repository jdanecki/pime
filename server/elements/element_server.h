#ifndef ELEMENT_SERVER_H
#define ELEMENT_SERVER_H
#include "../../core/alchemist/elements/element.h"

class ElementServer : public Element
{
  public:
    ElementServer(BaseElement * b);
    bool action(Product_action action, Player * pl) override;
    bool action_cut();
    bool action_hit();

    bool player_action(Player_action action, Player * pl) override;
    bool action_drink();
    bool action_eat();
    void show(bool details = true) override;
    bool can_pickup() override;
};
ElementServer * create_element(BaseElement * base);

#endif // ELEMENT_SERVER_H