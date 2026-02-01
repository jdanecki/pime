#ifndef ANIMAL_SERVER_H
#define ANIMAL_SERVER_H

#include "../../core/alchemist/elements/animal.h"
#include "being_server.h"

class AnimalServer : public Animal, public BeingServer
{
    int delay_for_move;
    int delay_for_grow;
    int dst_loc_x, dst_loc_y;

  public:
    void move();
    bool tick() override;

    AnimalServer(BaseAnimal * base);
    bool action(Product_action action, Player * pl) override;
    void show(bool details = true) override;
    bool grow() override;
    bool can_pickup() override;
    bool feed();
};
AnimalServer * create_animal(BaseAnimal * base);

#endif // ANIMAL_SERVER_H