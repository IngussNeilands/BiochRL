#ifndef INVENTORY_H
#define INVENTORY_H

class Item;
class Tile;
class Actor;
class Object;

class Inventory
{
    public:

        Actor* master;

        std::vector<Item*>* items;

        void add_item(Item*);
        void remove_item(Item*);
        int get_count();

        Inventory();
        ~Inventory();

        void drop_item(Item* item);

        int Inventory::get_total_weight();
};

#endif
