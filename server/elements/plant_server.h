#ifndef PLANT_SERVER_H
#define PLANT_SERVER_H

#include "../../core/alchemist/elements/plant.h"
#include "being_server.h"

class PlantServer : public Plant, public BeingServer
{
    int delay_for_grow;

  public:
    bool grow() override;

    PlantServer(BasePlant * base);

    void sow();
    void change_phase(Plant_phase p);
    bool action(Product_action action, Player * pl) override;
    void show(bool details = true) override;
    bool can_pickup() override;
    bool player_action(Player_action action, Player * pl) override;
    void set_phase(Plant_phase p);
};
PlantServer * create_plant(BasePlant * base);

#endif // PLANT_SERVER_H