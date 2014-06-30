#include "stdafx.h"
#include "event.h"
#include "actors\actor.h"
#include "item.h"
#include "tile.h"

Event::Event()
{

};

void Event::initialize()
{
    this->message = "UNSET";
    this->actors = NULL;
    this->items = NULL;
    this->tiles = NULL;
};

Event::Event(std::string message, actor_vec_t* actors, std::vector<Item*>* items, tile_vec_t* tiles)
{
    this->message = message;
    this->actors = actors;
    this->items = items;
    this->tiles = tiles;
};

void Event::activate()
{

};
