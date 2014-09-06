#pragma once

#ifndef GAME_H
#define GAME_H

#include "libtcod_cpp_hpp\mouse_types.h"
#include "libtcod_cpp_hpp\console_types.h"

#include <enums\gamestate.h>
#include <enums\screens.h>
#include <enums\spawntypes_t.h>
#include <statistics.h>

#include <vector>
#include <queue>
#include <map>
#include <enums/background_types.h>


class Map;
class Tile;

class Combat;

class Actor;
class Person;
class Troll;
class BadMother;
class Jackal;
class Skeleton;
class Ogre;

class Room;
class Item;
class DebugOptions;
class Ui;

class TCODConsole;
class TCODRandom;
class CustomKey;
class MenuItem;

class CompareQueueTicks
{
    public:
        bool operator() (Actor* left, Actor* right) const;
};

class Game
{
    public:
        static std::wstring main_name;
        static std::wstring term_name;

        static std::vector<std::string*>* menu_choices;
        static std::map<background_types_t, std::vector<MenuItem*>* >* menuitem_choices;

        static Person* player;     //the PC
        //static Ui* ui;
        static GameStates current_state;
        static Screens current_screen;
        static background_types_t current_background_type;
        static DebugOptions* debug_opts;

        static void debug_key_input();
        static TCOD_key_t key_evt;
        static TCOD_mouse_t mouse_evt;

        static std::vector<CustomKey*>* custom_keys;
        static CustomKey* custom_key1;
        static CustomKey* custom_key2;
        static CustomKey* custom_key3;
        static CustomKey* custom_key4;
        static CustomKey* custom_key5;
        static void init_custom_keys();
        static CustomKey* get_free_custom_key();

        static std::priority_queue<Actor*, std::vector<Actor*>, CompareQueueTicks>* game_queue;
        static void add_to_queue(Actor* actor);
        static unsigned int queue_ticks;

        static int fov_radius;
        static std::vector<Actor*> enemies;  
        static std::vector<Item*> items;  

        static int screen_w;
        static int screen_h;

        static int targetting_index;

        static int map_width;
        static int map_height;
        static int town_width;
        static int town_height;

        static int view_width;
        static int view_height;

        static int camera_w;
        static int camera_h;
        static int camera_x;
        static int camera_y;

        static void center_camera_on(int abs_x, int abs_y);
        static void center_camera_on(Actor* actor);
        static void center_camera_on(Tile* tile);
        static void center_camera_on_player();
        static void move_camera(int dir_x, int dir_y);

        static unsigned long int turn_count;
        static unsigned long long int tick_count;

        static int fps_limit; //how many frames do you want to refresh at a second

        static TCODConsole* game_console;
        static Map* current_map;
        static std::vector<Map*>* atlas;
        static int current_map_index;

        static bool buildmode;
        static Tile *clipboard;

        static TCODRandom* spawning_rng;
        static TCODRandom* spawning_ratio_rng;
        static TCODRandom* item_spawn_rng;
        static TCODRandom* linear_rng;
        static TCODRandom* event_rng;
        static TCODRandom* stat_rolls_rng;
        static TCODRandom* dungeon_builder_rng;

        static void init_rng();

        static Statistics* stats;

        static std::string last_cmd;

        // Game();
        static Map* build_world(int floor);
        static Map* build_town();
        static bool validate_town(Map* town);
        static void update();

        static void update_ui();

        static void draw_ui();
        static void draw_alerts();

        static void specify_player();
        static Person*  initialize_player();
        // static void  initialize_enemies();
        static void  initialize_items();
        static void fill_dungeon(Map* world);
        static void fill_generic_room(Room* room, Map* world);
        static void fill_town(Map* world);

        static Person * create_person(std::string name, int age, int x, int y, char repr, Map* map);
        static Person * create_townsmen(std::string name, int age, int x, int y, char repr, Map* map);
        template<class T>
            static T* create_creature(std::string name, int age, int x, int y, char repr, Map* world);

        static Tile* get_mouse_tile();
        static Tile* get_player_tile();

        static MonsterSpawnTypes get_spawn_type(int floor);
        template<class T>
            static T* spawn_creature(Room* room, std::string name, int age, char repr, Map* world);
        template<class T>
            static T* spawn_creature_ally(Tile* tile, std::string name, int age, char repr, Map* world);

        static void play_music();

        static void init_game();
        static void start_game();
        static void mainloop();
        static void init_engine();

        static void give_player_teleport(Actor* player);
        static void give_player_god_mode();


        static int __version_major;
        static int __version_minor;
        static int __version_mini;

        static std::string get_version();

        static void quit_game();
};


#endif
