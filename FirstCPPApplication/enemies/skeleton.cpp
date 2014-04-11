#include "stdafx.h"
#include "skeleton.h"

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

Skeleton::Skeleton(std::string name, int age, int x, int y, char repr) : Person(name, age, x, y, repr)
{
    this->xp_value = 50;
    // this->name = name;
    // this->age = age;
    // this->x = x;
    // this->y = y;

    // //set its representation, aka color and char
    // Representation * new_repr =   new Representation;
    // this->representation = new_repr;
    // this->representation->repr = repr;
    // this->representation->setFGColor(TCODColor::white, true, true, true);
    TCODRandom* rnd = TCODRandom::getInstance();
    float variant = rnd->getFloat(9.0f, 10.0f);
    //std::cout << (variant/10.0f) << std::endl;
    this->representation->setFGColor(TCODColor::white * (variant/10.0f), true, true, true);

    // Inventory* inventory =   new Inventory;
    // this->inventory = inventory;
    // this->equipment = new Equipment;
    // this->equipment->master = this;

    // is_fighter = true;

    // combat = new Combat("temp name", 50, this, 't' );
    // combat->assign_to_master(this);
    // has_live_combat = true;

    // this->pack_size = 10;
    // my_tile = NULL;
    this->attrs->health->max_val = 80;
    this->attrs->health->current_val = 80;

    this->attrs->damage->max_val = 5;
    this->attrs->damage->current_val = 5;

};

void Skeleton::championize()
{
    this->representation->setFGColor(TCODColor::white*(TCODColor::darkBlue-TCODColor::darkYellow), true, false, true);
    this->attrs->health->current_val+=this->attrs->health->current_val;
    this->attrs->health->max_val+=this->attrs->health->max_val;
    this->attrs->damage->current_val+=this->attrs->damage->current_val;
    this->attrs->damage->max_val+=this->attrs->damage->max_val;
    this->xp_value= this->xp_value*1.5;

};
