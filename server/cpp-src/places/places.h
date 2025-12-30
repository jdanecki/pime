#include "../elements_server.h"
#include "../networking.h"

#ifndef PLACES_H
#define PLACES_H

#define GENERATE_PLACE(name, body)                   \
class name : public PlaceServer, public Networked    \
{                                                     \
    name(Place_id id);                               \
    public:                                           \
      static PlaceServer * create##name(Place_id id); \
        body                                          \
};

GENERATE_PLACE(Field, bool action(Product_action, Player *);)

PlaceServer * create_place(Place_id id);
#endif
