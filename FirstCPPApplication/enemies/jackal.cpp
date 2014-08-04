#include "stdafx.h"
#include "jackal.h"

#include <iostream>
#include <sstream>

// #include "libtcod.hpp"

#include <actors/person.h>
#include "thinker.h"
#include "inventory.h"
#include "equipment.h"
#include "Representation.h"
#include "tile.h"	
#include "attribute.h"
#include "attribute_container.h"
#include "combat.h"

Jackal::Jackal(std::string name, int age, int x, int y, char repr) : Person(name, age, x, y, repr)
{
    this->xp_value = 7;
    this->cls_name = "Jackal";
    // this->speed = 150;
    // this->name = name;
    // this->age = age;
    // this->x = x;
    this->img_path = get_data_path()+"img/jackal8x10.png";
    // this->y = y;

    //set its representation, aka color and char
    // Representation * new_repr =   new Representation;
    // this->representation = new_repr;
    // this->representation->repr = repr;
    TCODRandom* rnd = TCODRandom::getInstance();
    float variant = rnd->getFloat(5.0f, 10.0f);
    //std::cout << (variant/10.0f) << std::endl;
    this->representation->setFGColor(TCODColor::orange * (variant/10.0f), true, true, true);

    // Inventory* inventory =   new Inventory;
    // this->inventory = inventory;
    // this->equipment = new Equipment;
    // this->equipment->master = this;

    // is_fighter = true;

    // combat = new Combat("temp name", 50, this, 't' );
    // combat->assign_to_master(this);
    // has_live_combat = true;

    // my_tile = NULL;
    //this->pack_size = 20;

    this->attrs->health->max_val = 20;
    this->attrs->health->current_val = 20;

    this->attrs->damage->max_val = 2;
    this->attrs->damage->current_val = 2;

};

void Jackal::championize()
{
    Person::championize();
    // the_jackal = Game::create_jackal("Burly Jackal", 31, creature_x, creature_y, 'j', world, "jackal combat");
    this->representation->setFGColor(TCODColor::sepia+TCODColor::sepia+TCODColor::darkYellow, true, false, true);
    this->attrs->health->current_val+=this->attrs->health->current_val;
    this->attrs->health->max_val+=this->attrs->health->max_val;
    this->xp_value = (int)std::floor(this->xp_value*1.5);

};
