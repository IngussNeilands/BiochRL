#pragma once
#ifndef DROP_HANDLER_H
#define DROP_HANDLER_H


class Item;
class Actor;
class Tile;

class DropHandler
{
    public:
        static Item* handle_for_actor(Actor* actor);

};
#endif
