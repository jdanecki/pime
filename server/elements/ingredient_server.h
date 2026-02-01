#ifndef INGREDIENT_SERVER_H
#define INGREDIENT_SERVER_H

#include "../../core/alchemist/elements/ingredient.h"

class IngredientServer : public Ingredient
{
  public:
    size_t el;
    IngredientServer(InventoryElement * from, Ingredient_id id, Form f);
    bool action(Product_action action, Player * pl) override;
    bool can_pickup() override;
    void show(bool details = true) override;
};

#endif // INGREDIENT_SERVER_H