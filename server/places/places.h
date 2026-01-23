#include "../elements_server.h"
#include "../networking.h"

#ifndef PLACES_H
#define PLACES_H

#define GENERATE_PLACE(name, body)                                                                                                                                                                     \
    class name : public PlaceServer, public Networked                                                                                                                                                  \
    {                                                                                                                                                                                                  \
        name(Place_id id);                                                                                                                                                                             \
                                                                                                                                                                                                       \
      public:                                                                                                                                                                                          \
        static PlaceServer * create##name(Place_id id);                                                                                                                                                \
        name##_states state;                                                                                                                                                                           \
        void show_state() override;                                                                                                                                                                    \
        body                                                                                                                                                                                           \
    };

GENERATE_PLACE(Field, bool action(Product_action, Player *) override;)
GENERATE_PLACE(Barn, bool action(Product_action, Player *) override;)

PlaceServer * create_place(Place_id id);
#endif
