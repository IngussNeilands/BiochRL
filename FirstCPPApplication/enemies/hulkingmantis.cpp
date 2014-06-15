#include "stdafx.h"
#include "hulkingmantis.h"


#include <algorithm>
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
#include "ui.h"
#include "messages.h"

HulkingMantis::HulkingMantis(std::string name, int age, int x, int y, char repr) : Person(name, age, x, y, repr)
{
    this->xp_value = 100;
    this->cls_name = "HulkingMantis";
    // this->name = name;
    // this->age = age;
    // this->x = x;
    // this->y = y;

    //set its representation, aka color and char
    // Representation * new_repr =   new Representation;
    // this->representation = new_repr;
    // this->representation->repr = repr;
    TCODRandom* rnd = TCODRandom::getInstance();
    float variant = rnd->getFloat(5.0f, 10.0f);
    //std::cout << (variant/10.0f) << std::endl;
    this->representation->setFGColor(TCODColor::darkTurquoise * (variant/10.0f), true, true, true);
    this->img_path = get_data_path()+"img/hulkingmantis8x10.png";

    // Inventory* inventory =   new Inventory;
    // this->inventory = inventory;
    // this->equipment = new Equipment;
    // this->equipment->master = this;

    // is_fighter = true;

    // combat = new Combat("temp name", 50, this, 't' );
    // combat->assign_to_master(this);
    // has_live_combat = true;

    // my_tile = NULL;
    // this->pack_size = 10;
    this->attrs->health->max_val = 110;
    this->attrs->health->current_val = 110;

    this->attrs->damage->max_val = 13;
    this->attrs->damage->current_val = 13;

    this->attrs->armor->max_val = 3;
    this->attrs->armor->current_val = 3;


};

void HulkingMantis::championize()
{
    Person::championize();
    this->representation->setFGColor(TCODColor::green+TCODColor::green+TCODColor::darkYellow, true, false, true);
    this->attrs->health->current_val+=this->attrs->health->current_val;
    this->attrs->health->max_val+=this->attrs->health->max_val;
    this->xp_value= (int)floor(this->xp_value*1.5);
}

void HulkingMantis::Die() 
{
    Actor::Die();
};
