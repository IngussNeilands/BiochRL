#include "stdafx.h"
#include "ogre.h"

#include <iostream>
#include <sstream>
#include <algorithm>

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

Ogre::Ogre(std::string name, int age, int x, int y, char repr) : Person(name, age, x, y, repr)
{
    this->cls_name = "Ogre";
    this->xp_value = 55;
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
    this->representation->setFGColor(TCODColor::darkestTurquoise * (variant/10.0f), true, true, true);
    this->img_path = get_data_path()+"img/ogre10x10.png";

    // Inventory* inventory =   new Inventory;
    // this->inventory = inventory;
    // this->equipment = new Equipment;
    // this->equipment->master = this;

    // is_fighter = true;

    // combat = new Combat("temp name", 50, this, 't' );
    // combat->assign_to_master(this);
    // has_live_combat = true;

    // my_tile = NULL;
    // this->pack_size = 4;

    this->attrs->health->max_val = 100;
    this->attrs->health->current_val = 100;

    this->attrs->damage->max_val = 6;
    this->attrs->damage->current_val = 6;
};

void Ogre::championize()
{
    Person::championize();
    this->representation->setFGColor(TCODColor::white*(TCODColor::darkGrey-TCODColor::darkYellow), true, false, true);
    this->attrs->health->current_val+=this->attrs->health->current_val;
    this->attrs->health->max_val+=this->attrs->health->max_val;
    this->attrs->damage->current_val+=this->attrs->damage->current_val;
    this->attrs->damage->max_val+=this->attrs->damage->max_val;
    this->xp_value= (int)std::floor(this->xp_value*1.5);
}
