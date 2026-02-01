#ifndef SCROLL_SERVER_H
#define SCROLL_SERVER_H

#include "../../core/alchemist/elements/scroll.h"

class ScrollServer : public Scroll
{
  public:
    ScrollServer(Base * base);
    bool can_pickup() override;
    bool player_action(Player_action action, Player * pl) override;
};
ScrollServer * create_scroll(Base * base);
#endif // SCROLL_SERVER_H