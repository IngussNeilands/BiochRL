#pragma once

#ifndef GAME_H
#define GAME_H

#include "stdafx.h"
// #include "input.h"
#include "libtcod_cpp_hpp\libtcod.hpp"

#include <Vector>

class Map;
class Tile;
class Person;
class Pet;
class Actor;
class Item;

class Ui;

class Game
{
    public:
        Person* player;     //the PC
        Ui* ui;

        //player input
        TCOD_key_t key_evt;
        TCOD_mouse_t mouse_evt;

        int enemies_size ;   //don't really know how else to get the size of the
        // enemies list.  sizeof(type_inst_array)/sizeof(type) maybe.
        std::vector<Actor*> enemies;  //later, this will be an array of all the enemies 
        std::vector<Item*> items;  //later, this will be an array of all the enemies 

        int screen_w;
        int screen_h;

        unsigned long int turn_count;
        unsigned long long int tick_count;

        int fps_limit; //how many frames do you want to refresh at a second

        Map *world;
        Map *current_map;
        int current_map_index;

        bool buildmode;
        Tile *clipboard;

        std::string last_cmd;

        Game();
        void buildworld();
        void update();

        void update_ui();

        void draw_ui();

        Person*  initialize_player();
        void  initialize_enemies();
        void  Game::initialize_items();

        Person * create_person(std::string name, int age, int x, int y, char repr, std::string pet_name = "");


        void mainloop();
};

#endif
