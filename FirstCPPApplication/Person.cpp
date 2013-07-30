#include "stdafx.h"
#include "tile.h"
#include "world.h"
#include "Person.h"

#include <iostream>
// #include <sstream>
// #include <fstream>

Person::Person(){
    is_fighter = true;

    pet = new Pet;
    pet->master = this;
    has_live_pet = true;

    my_tile = NULL;
};
Person::~Person(){
    //TODO: safely remove pet
};
void Person::update()
{
    if (has_live_pet == true)
    {
        pet->update();
        if (pet->was_attacked == true)
        {
            //retaliate

            //calm the pet down
            pet->was_attacked = false;
        }
    };

};

void Person::putPerson( Tile * next_tile, int new_x, int new_y)
{	//puts a person on a tile, resets the old tile

    if (my_tile != NULL){
        // my_tile->occupant = NULL;
        my_tile->makeUnoccupied(this); 
    }

    if (next_tile != NULL){
        //next_tile->occupant = this;
        //next_tile->occupants->push_back(this);
        this->my_tile = next_tile;
        next_tile->makeOccupied(this) ; 
        // cout << "new occupant: " << name << endl;
    }
    else if (next_tile == NULL)
    {
        cout << "next tile is null" << endl;
    }
    else
    {
        cout << "else, this shouldn't be a possiblity" << endl;
    }

    if (new_x != NULL && new_y != NULL)
    {
        x = new_x;
        y = new_y;
    }

};


void Person::attack(Actor * target)
{
    pet->Attack(((Person*)target)->pet, 10); //shit will happen if the target isn't a Person
};
