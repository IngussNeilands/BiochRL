#include "stdafx.h"

#include <iostream>
#include <sstream>

#include "Person.h"

// #include "libtcod.hpp"

#include "thinker.h"
#include "inventory.h"
#include "equipment.h"
#include "Representation.h"
#include "tile.h"	
#include "attribute.h"
#include "attribute_container.h"
#include "combat.h"
#include "messages.h"
#include "game.h"
#include "ui.h"
#include "civilian.h"
#include "enums\hunger_threshold.h"
#include <utils.h>

Person::Person(std::string name, int age, int x, int y, char repr)
{
    this->cls_name = "Person";
    this->name = name;
    this->age = age;
    this->x = x;
    this->y = y;

    this->img_path = get_data_path()+"img/townsmen8x8.png";

    //   if (name == "Josh")
    //{
    //   new Message(Ui::msg_handler_main, NOTYPE_MSG, "Creating the Josh player.");
    //}

    this->xp = 0;
    this->xp_this_level = 0;
    this->xp_required_to_lvlup = 100;
    this->xp_value = 10;
    this->level = 1;

    this->is_champion = false;
    this->pack_size = 0;


    //set its representation, aka color and char
    this->representation = new Representation;
    this->representation->repr = repr;
    this->representation->setFGColor(TCODColor::lighterFlame, true, true, true);

    Inventory* inventory =   new Inventory();
    this->inventory = inventory;
    this->inventory->master = this;
    this->equipment = new Equipment;
    this->equipment->master = this;

    is_fighter = true;

    combat = new Combat("temp name", 50, this, 't' );
    combat->assign_to_master(this);
    has_live_combat = true;

    my_tile = NULL;

    is_active = true;

    this->attrs->damage->max_val = 1;
    this->attrs->damage->current_val = 1;

};

Person::Person() : Actor()
{
    this->cls_name = "Person";
    this->name = "Unset Person name";
    this->age = 80085;
    this->x = 10;
    this->y = 10;

    this->dest_x = -1;
    this->dest_y = -1;


    //set its representation, aka color and char
    this->representation->repr = '~';

    Inventory* inventory =   new Inventory();
    this->inventory = inventory;
    this->inventory->master = this;
    this->equipment = new Equipment;
    this->equipment->master = this;

    is_fighter = true;

    combat = new Combat("temp2 name", 25, this, 't');
    combat->assign_to_master(this);
    has_live_combat = true;

    my_tile = NULL;
};

Person::~Person(){
    //TODO: safely remove Combat
    this->my_tile->makeUnoccupied(this);
};

void Person::update()
{
    //apply item basic item attributes of only equipped items if they've not
    //been equipped already
    this->equipment->Update();

    //update hunger
    Attribute* hunger = this->attrs->hunger;
    if (hunger->get_val_percentage() > WastingAwayHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.25;
    }
    else if (hunger->get_val_percentage() > NearDeathHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.1;
    }
    else if (hunger->get_val_percentage() > FamishedHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.05;
    }
    else if (hunger->get_val_percentage() > StarvingHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.025;
    }
    // else if (hunger->get_val_percentage() > VeryHunger)
    // {
    //     this->attrs->health->regen_rate -= (float)this->attrs->health->max_val*0.25;
    // }
    // else if (hunger->get_val_percentage() > DefaultHunger)
    // {
    //     this->attrs->health->regen_rate -= (float)this->attrs->health->max_val*0.05;
    // }

    //item buffs
    std::vector<TimedEffect*>* timed_items = this->timed_attr_effects;
    auto it = timed_items->begin();
    for (it; it != timed_items->end(); )
    {
        if ((*it)->is_expired(Game::turn_count))
        {
            (*it)->effect->RemoveAllEffects(this);
            delete *(it);
            it = timed_items->erase(it);
        }
        else
        {
            it++;
        }
    }

    //spell bufs
    std::vector<TimedEffect*>* timed_spells = this->timed_spell_effects;
    it = timed_spells->begin();
    for (it; it != timed_spells->end();)
    {
        if ((*it)->is_expired(Game::turn_count))
        {
            (*it)->effect->RemoveAllEffects(this);
            delete *(it);
            it = timed_spells->erase(it);
        }
        else
        {
            it++;
        };
    }

    this->attrs->Update();

    //TODO intervals

    if (this->thinker != NULL)
    {
        thinker->update();

        //Combat UPDATE
        if (has_live_combat == true) {
            combat->update();
        }
        else {
            printf("no combat\n");
        }
    }

    if (this->combat != NULL)
    {
        this->combat->try_to_die();
    };
};

void Person::attack(Actor * target)
{
    //only Persons and children have combats
    Damage* dmg = new Damage();
    dmg->normal = this->attrs->damage->current_val;
    if (this->is_sneaking && target->thinker!=NULL && target->thinker->get_is_aware() == false)
    {
        dmg->normal = dmg->normal*1.5;
    };
    combat->attack(((Person*)target)->combat, dmg); 
    delete dmg;
};

void Person::championize()
{
    this->name = "Champion "+this->name;
    this->is_champion = true;
}

std::string Person::talk_wrap(Actor* target, std::string text)
{
    return "He says, \""+text+"\"";
};

//incr turn
bool Person::talk_to(Actor* target)
{
    if (target->thinker->civilian != NULL)
    {
        std::string text = this->talk_wrap(target, target->thinker->civilian->talk_general_topic());
        new Message(Ui::msg_handler_main, CHAT_MSG, colfg(TCODColor::lighterAmber, text));

        if (target->thinker->civilian->is_shopkeep)
        {
            //get surrounding items
            tile_vec_t* adj_tiles = target->my_tile->getAdjacentTiles(1);
			item_vec_t* items = new item_vec_t;
            for (tile_vec_t::iterator it = adj_tiles->begin(); it!=adj_tiles->end(); it++)
            {
				Tile* tile = *it;
				auto on_floor = new item_vec_t(*tile->inventory->items);
				if (on_floor->empty()) { continue; }
				for (auto item = on_floor->begin(); item != on_floor->end(); item++)
				{
					tile->inventory->remove_item(*item);
				}
				items->insert(items->end(), on_floor->begin(), on_floor->end());

                delete on_floor;
			};
			delete adj_tiles;

			std::cout << "items on floor: " << items->size() << std::endl;

            //remove them
            //replace them with money
            int total_gold = 0;
            for (auto it = items->begin(); it != items->end(); it++)
            {
                delete *it;
                total_gold += 10;
            };

            this->total_gold += total_gold;
			std::cout << "you just earned: " << total_gold << std::endl;
			std::cout << "you now have: " << this->total_gold << std::endl;
        };

        return true;
    }

    return false;
};
