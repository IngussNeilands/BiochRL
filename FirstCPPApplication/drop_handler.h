#pragma once
#ifndef DROP_HANDLER_H
#define DROP_HANDLER_H


class Item;
class Actor;
class Tile;

class DropHandler
{
    public:
        static Item* item_drop_handler(Actor* actor);

};
#endif
