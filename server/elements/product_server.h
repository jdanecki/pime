#ifndef PRODUCT_SERVER_H
#define PRODUCT_SERVER_H

#include "../../core/alchemist/elements/product.h"

class ProductServer : public Product
{
  public:
    void * padding;
    int ing_count;
    size_t ings[2];

    void init(int c, Form f);
    ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id id, Form f, int act_cnt);
    // ProductServer(InventoryElement ** from, int count, Product_id id, Form f, int act_cnt);
    void show(bool details = true) override;
    virtual bool use_on(InventoryElement * object, Player * pl);
    virtual bool use_tile(int map_x, int map_y, int x, int y, Player * pl);
    bool can_pickup() override;
};

#endif // PRODUCT_SERVER_H