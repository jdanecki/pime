void use()
{
    printf("Which item do you want to use?\n");
    InventoryElement * el = select_element(&player->inventory);
    if (!el)
        return;
    ProductServer * product = dynamic_cast<ProductServer *>(el);
    if (!product)
    {
        printf("You can't use %s, make product from it\n", el->get_name());
        return;
    }
    printf("On what do you want to use %s\n", el->get_name());

    printf("%se/E - elements (details off/on)\n", colorCyan);
    printf("p/P - plants (details off/on)\n");
    printf("a/A - animals (details off/on)\n");
    printf("n/N - npcs/player (details off/on)\n");
    printf("%s%s", colorNormal, colorGreenBold);

    InventoryElement * obj;
    char c = wait_key('s');
    switch (c)
    {
        case 'e':
        case 'E':
            obj = select_element(elements);
            break;

        case 'a':
        case 'A':
            obj = select_element(animals);
            break;

        case 'p':
        case 'P':
            obj = select_element(plants);
            break;

        case 'n':
        case 'N':
            obj = select_element(npcs);
            break;
    }
    if (!obj)
        return;

    product->use(obj, player);

    //  player->inventory->remove(el);
}


