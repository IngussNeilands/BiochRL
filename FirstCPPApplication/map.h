#pragma once

#ifndef WORLD_H
#define WORLD_H

#define     TILE_NORMAL     0
#define     TILE_WALL       1

#include <string>
#include <vector>

#include "object.h"


class Game;
class Room;
class Person;
class Object;
class Tile;
class Actor;
class Event;

class TCODMap;

class Map : public Object
{
    public:
        Map();
        ~Map();

        actor_vec_t enemies;
        actor_vec_t allies;  
        std::vector<Event*>* events;

        int width, height;
        int stair_x, stair_y;
        int depth;
        std::string description; // default description if tile does not have one
        bool pos_in_map(int x, int y);
        bool has_hero_spawned;

        int build_dungeon_from_random(int seed, int floor);
        int build_town_from_random(int seed);

        Room* build_circle_room(int room_x, int room_y,
                int room_width, int room_height, int door_index);
        Room* build_rect_room(int room_x, int room_y,
                int room_width, int room_height, int door_index);

        void update();
        void Map::spawn_hero();
        bool Map::should_spawn_hero();
        int draw();
        bool attackMovePlayer(Person *thePerson, int x2, int y2);

        std::vector< std::vector<Tile> > *tileVector;
        std::vector<Room*>* roomVector;
        std::vector<Vec2i>* inside_tiles;
        Tile* getTileAt(int x, int y, bool is_original_pos=true, int ox=-1, int oy=-1);

        std::vector<Map*>* floors_above;
        std::vector<Map*>* floors_below;

        TCODMap* l_map;
};


#endif
