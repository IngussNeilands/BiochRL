#include "stdafx.h"

#include "inventory.h"
#include "item.h"
#include "actors\actor.h"
#include "object.h"
#include "tile.h"
#include "equipment.h"
#include "spells.h"
#include "custom_key.h"

Inventory::Inventory()
{
    this->items = new std::vector<Item*>;
    this->master = NULL;
};

void Inventory::add_item(Item* item)
{

    if (item->spell_effect != NULL)
    {
        item->spell_effect->master = this->master;
    };

    //check for item in list, if its not, add it in
    auto it = std::find(this->items->begin(), this->items->end(), item);
    if (it == this->items->end())
    {
        this->items->push_back(item);
    };

};

void Inventory::remove_item(Item* item)
{
    if (this->master != NULL && this->master->equipment->is_item_equipped(item))
    {
        this->master->equipment->unequip_item(item);
    };
    //check for item in list, if its not, remove it 
    auto it = std::find(this->items->begin(), this->items->end(), item);
    if (it != this->items->end())
    {
        this->items->erase(it);
    };
};

void Inventory::drop_item(Item* item)
{
    if (item->spell_effect != NULL)
    {
        item->spell_effect->master = NULL;

        //loop through customkeys and unassign
        ckey_vec_t_it it = Game::custom_keys->begin();
        for (it; it != Game::custom_keys->end(); it++)
        {
            if ((*it)->element == item)
            {
                (*it)->reset_state();
            };
        }
    };
    this->remove_item(item);
    if (master != NULL)
    {
        this->master->my_tile->place_item_down(item);
    };

};

int Inventory::get_count()
{
    return this->items->size();
};

int Inventory::get_total_weight()
{
    int total_weight = 0;
    for (std::vector<Item*>::iterator it = this->items->begin(); it != this->items->end(); ++it)
    {
        total_weight += (*it)->weight;
    }
    return total_weight;
};
