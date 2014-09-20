#pragma once
#ifndef MAP_BUILDER_H
#define MAP_BUILDER_H
#include <enums/spawntypes_t.h>

class Map;
class Room;

class MapBuilder
{
    public:
        void fill_dungeon(Map* world);
        void fill_generic_room(Room* room, Map* world);
        MonsterSpawnTypes get_spawn_type(int floor);

};
#endif
