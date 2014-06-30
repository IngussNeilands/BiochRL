#pragma once
#ifndef EVENT_H
#define EVENT_H

class Actor;
class Item;
class Tile;

class Event
{
    public:
        bool is_active;

        std::string message; //once the event happens, thisll get posted
        actor_vec_t* actors;
        std::vector<Item*>* items;
        tile_vec_t* tiles;

        Event();
        Event(std::string message, actor_vec_t* actors, std::vector<Item*>* items, tile_vec_t* tiles);

        void activate();
        void initialize();
};

#endif
