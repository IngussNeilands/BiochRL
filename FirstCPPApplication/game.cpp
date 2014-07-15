#include "stdafx.h"
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <assert.h>
#include <algorithm>

#include <SDL.h>
#include <SDL_mixer.h>

#include "libtcod.hpp"

#include "enums\spawntypes_t.h"
#include "randsys.h"
#include "game.h"
#include <actors/person.h>
#include "map.h"
#include "FirstCPPApplication.h"
#include "utils.h"
#include "Representation.h"
#include "build.h"
#include "input.h"
#include "map.h"
#include "thinker.h"
#include "item.h"
#include "inventory.h"
#include "tile.h"	
#include "combat.h"
#include "ui.h"
#include "attribute_container.h"
#include "attribute.h"
#include "enemies\troll.h"
#include "enemies\jackal.h"
#include "enemies\skeleton.h"
#include "enemies\ogre.h"
#include <enums\gamestate.h>
#include "attr_effect.h"
#include "Room.h"
#include "debug_options.h"
#include "messages.h"
#include <enemies\bad_mother.h>
#include "civilian.h"
#include "enums\screens.h"
#include "spells.h"
#include "class.h"
#include <enemies\hulkingmantis.h>
#include <enemies\idol.h>
#include <enemies\imp.h>
#include <enemies\mutantfish.h>
#include <enemies\lizard.h>
#include <enemies\crazedcook.h>
#include <enemies\wildling.h>
#include <enemies\sludge.h>
#include <enemies\jumper.h>
#include "utils.h"
#include <equipment.h>
#include <enums\slots_t.h>
#include "custom_key.h"
#include "parser.h"


int Game::__version_major = 0;
int Game::__version_minor = 11;
int Game::__version_mini = 2;


// Game initialization
DebugOptions* Game::debug_opts = new DebugOptions;

std::wstring Game::main_name = L"BiochRL++";
std::wstring Game::term_name = L"Game Terminal";

int Game::screen_w = 80; //the average RL resolution
int Game::screen_h = 50;

int Game::targetting_index = 1;


ckey_vec_t* Game::custom_keys;
CustomKey* Game::custom_key1 = new CustomKey;
CustomKey* Game::custom_key2 = new CustomKey;
CustomKey* Game::custom_key3 = new CustomKey;
CustomKey* Game::custom_key4 = new CustomKey;
CustomKey* Game::custom_key5 = new CustomKey;

int Game::map_width = 60;
int Game::map_height = 40;
int Game::town_width = 60;
int Game::town_height = 40;

int Game::view_width = 60;
int Game::view_height = 40;

int Game::camera_w = Game::view_width;
int Game::camera_h = Game::view_height;
int Game::camera_x = 0;
int Game::camera_y = 0;

int Game::fov_radius = 6;
bool Game::buildmode = false;
int Game::fps_limit= 120; //default
TCODConsole* Game::game_console = new TCODConsole(Game::map_width, Game::map_height);

std::string Game::last_cmd = "not set";

GameStates Game::current_state = GameStates::GameplayState;
Screens Game::current_screen = Screens::InventoryScreenType;

Person* Game::player = NULL;     //the PC

TCOD_key_t Game::key_evt;
TCOD_mouse_t Game::mouse_evt;

actor_vec_t Game::enemies = actor_vec_t();  //later, this will be an array of all the enemies 
std::vector<Item*> Game::items = std::vector<Item*>();  //later, this will be an array of all the enemies 

unsigned long int Game::turn_count = 1;
unsigned long long int Game::tick_count = NULL;

//Map* Game::world = NULL;
Map* Game::current_map = NULL;
int Game::current_map_index = NULL;
std::vector<Map*>* Game::atlas = new std::vector<Map*>;
Tile* Game::clipboard = NULL;

TCODRandom* Game::spawning_rng = new TCODRandom();
TCODRandom* Game::item_spawn_rng = new TCODRandom();
TCODRandom* Game::linear_rng = new TCODRandom();
TCODRandom* Game::event_rng = new TCODRandom();
TCODRandom* Game::stat_rolls_rng = new TCODRandom();
TCODRandom* Game::dungeon_builder_rng = new TCODRandom();

Statistics* Game::stats = new Statistics();


std::string Game::get_version()
{
    std::stringstream ss;
    ss << "v" << Game::__version_major << "." << Game::__version_minor << "." << Game::__version_mini;
    return ss.str();
};

MonsterSpawnTypes Game::get_spawn_type(int floor)
{
    RandomWeightMap<MonsterSpawnTypes> rwm = RandomWeightMap<MonsterSpawnTypes>();
    if (floor == 1)
    {
        rwm.add_item(TrollSpawn, 10);
        rwm.add_item(JackalSpawn, 10);
        rwm.add_item(SkeletonSpawn, 2);
        rwm.add_item(IdolSpawn, 5);
    }
    else if (floor == 2)
    {
        rwm.add_item(IdolSpawn, 10);
        rwm.add_item(MutantFishSpawn, 10);
        rwm.add_item(ImpSpawn, 5);
        rwm.add_item(CrazedCookSpawn, 5);
        rwm.add_item(HulkingMantisSpawn, 1);
    }
    else if (floor == 3)
    {
        rwm.add_item(ImpSpawn, 10);
        rwm.add_item(CrazedCookSpawn, 10);
        rwm.add_item(HulkingMantisSpawn, 10);
        rwm.add_item(WildlingSpawn, 5);
        rwm.add_item(SludgeSpawn, 5);
        rwm.add_item(JumperSpawn, 1);
    }
    else if (floor == 4)
    {
        rwm.add_item(CrazedCookSpawn, 10);
        rwm.add_item(WildlingSpawn, 10);
        rwm.add_item(SludgeSpawn, 10);
        rwm.add_item(JumperSpawn, 5);

    }
    else if (floor >= 5)
    {
        rwm.add_item(CrazedCookSpawn, 5);
        rwm.add_item(WildlingSpawn, 5);
        rwm.add_item(SludgeSpawn, 5);
        rwm.add_item(JumperSpawn, 10);
        rwm.add_item(OgreSpawn, 10);
        rwm.add_item(BadMotherSpawn, 2);

    };

    return rwm.get_item(Game::spawning_rng);
};

Tile* Game::get_mouse_tile()
{
    Tile* mouse_tile = Game::current_map->getTileAt(Game::mouse_evt.cx+Game::camera_x, Game::mouse_evt.cy+Game::camera_y);
    return mouse_tile;
};

Tile* Game::get_player_tile()
{
    Tile* player_tile = Game::player->my_tile;
    return player_tile;
};

void Game::fill_town(Map* world)
{
    Person* the_townsmen = Game::create_townsmen("Random Townsmen", 30, 10, 10, 't', world);
    world->enemies.push_back(the_townsmen);
};

void Game::fill_generic_room(Room* room, Map* world)
{
    int floor = world->depth;
    // if (Game::current_map == NULL) { floor = 1; }
    // else { floor = Game::current_map->depth; };
    MonsterSpawnTypes spawn_type = get_spawn_type(floor);
    if (spawn_type == MonsterSpawnTypes::TrollSpawn)
    {
        spawn_creature<Troll>(room, "Troll", 34, 'T', world);
    }
    else if (spawn_type == MonsterSpawnTypes::JackalSpawn)
    {
        spawn_creature<Jackal>(room, "Jackal", 31, 'j', world);
    }
    else if (spawn_type == MonsterSpawnTypes::HulkingMantisSpawn)
    {
        spawn_creature<HulkingMantis>(room, "HulkingMantis", 31, 'm', world);
    }
    else if (spawn_type == MonsterSpawnTypes::IdolSpawn)
    {
        spawn_creature<Idol>(room, "Idol", 31, 'i', world);
    }
    else if (spawn_type == MonsterSpawnTypes::ImpSpawn)
    {
        spawn_creature<Imp>(room, "Imp", 31, 'i', world);
    }
    else if (spawn_type == MonsterSpawnTypes::MutantFishSpawn)
    {
        spawn_creature<MutantFish>(room, "MutantFish", 31, 'f', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SpinyLizardSpawn)
    {
        spawn_creature<SpinyLizard>(room, "SpinyLizard", 31, 'l', world);
    }
    else if (spawn_type == MonsterSpawnTypes::CrazedCookSpawn)
    {
        spawn_creature<CrazedCook>(room, "CrazedCook", 31, 'c', world);
    }
    else if (spawn_type == MonsterSpawnTypes::WildlingSpawn)
    {
        spawn_creature<Wildling>(room, "Wildling", 31, 'w', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SludgeSpawn)
    {
        spawn_creature<Sludge>(room, "Sludge", 31, 's', world);
    }
    else if (spawn_type == MonsterSpawnTypes::JumperSpawn)
    {
        spawn_creature<Jumper>(room, "Jumper", 31, 'j', world);
    }
    else if (spawn_type == MonsterSpawnTypes::OgreSpawn)
    {
        spawn_creature<Ogre>(room, "Ogre", 103, 'O', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SkeletonSpawn)
    {
        spawn_creature<Skeleton>(room, "Skeleton", 92, 's', world);
    }
    else if (spawn_type == MonsterSpawnTypes::BadMotherSpawn)
    {
        spawn_creature<BadMother>(room, "BadMother", 92, 'B', world);
    }
};

void Game::fill_dungeon(Map* world)
{
    //fill rooms with enemies and monsters
    for (std::vector<Room*>::iterator it = world->roomVector->begin(); it != world->roomVector->end(); ++it)
    {
        if (it == world->roomVector->begin())
        {
            //spawn one dude to whom you can sell your shit
            Person* the_townsmen = Game::create_townsmen("Travelling Salesman", 30, 10, 10, 't', world);
            world->allies.push_back(the_townsmen);

        }
        else
        {
            fill_generic_room(*it, world);
        }
    }

};

    template<class T>
T* Game::spawn_creature(Room* room, std::string name, int age, char repr, Map* world)
{
    int enemy_count = Game::spawning_rng->getInt(1, T::pack_size, T::preferred_pack_size);
    enemy_count = min((int)T::pack_size, (int)enemy_count);
    for (int i = 0; i < enemy_count; i++)
    {
        int creature_x, creature_y;
        creature_x = Game::linear_rng->getInt(2, room->width-3) + room->x;
        creature_y = Game::linear_rng->getInt(2, room->height-3) + room->y;

        if (!world->getTileAt(creature_x, creature_y)->is_walkable()) {continue;};

        T* the_creature = Game::create_creature<T>(name, age, creature_x, creature_y, repr, world);
        if (linear_rng->getInt(1, 100) < 10) 
        {
            the_creature->championize();
        };

        world->enemies.push_back(the_creature);
    }
    return NULL;
};

    template<class T>
T* Game::spawn_creature_ally(Tile* tile, std::string name, int age, char repr, Map* world)
{
    // int enemy_count = Game::spawning_rng->getInt(1, T::pack_size, T::preferred_pack_size);
    // for (int i = 0; i <= enemy_count; i++)
    // {
    int creature_x, creature_y;
    creature_x = tile->tile_x;
    creature_y = tile->tile_y;

    if (!world->getTileAt(creature_x, creature_y)->is_walkable()) {return NULL;};

    T* the_creature = Game::create_creature<T>(name, age, creature_x, creature_y, repr, world);
    if (linear_rng->getInt(1, 100) < 10) 
    {
        the_creature->championize();
    };

    world->allies.push_back(the_creature);
    the_creature->thinker->is_ally = true;
    return the_creature;
    // }
};
template Skeleton * Game::spawn_creature_ally<Skeleton>(Tile* tile, std::string name, int age, char repr, Map* world);

bool Game::validate_town(Map* town)
{
    return false;
    if (town == NULL)
    {
        return false;
    };

    return true;
};

Map* Game::build_town()
{
    Map* world = new Map;
    world->build_town_from_random(0);
    Game::fill_town(world);

	std::cout << "going in " << std::endl;
    while (!validate_town(world))
    {
	std::cout << "\t\tlooping " << std::endl;
        delete world;
        world = new Map;
        world->build_town_from_random(0);
        Game::fill_town(world);
    };
    return world;
}

Map* Game::build_world(int floor)
{

    Map* world = new Map;
    world->build_dungeon_from_random(0, floor);

    //set the colors
    if (floor == 1)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::desaturatedRed, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestRed, true, false, true);
    }
    else if (floor == 2)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::desaturatedBlue, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestBlue, true, false, true);
    }
    else if (floor == 3)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::desaturatedGreen, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestGreen, true, false, true);
    }
    else if (floor == 4)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::desaturatedYellow, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestYellow, true, false, true);
    }
    else if (floor == 5)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::desaturatedMagenta, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestMagenta, true, false, true);
    }
    else if (floor == 6)
    {
        Tile::FloorType->representation->setFGColor(TCODColor::grey, true, false, true);
        Tile::WallType->representation->setFGColor(TCODColor::darkestGrey, true, false, true);
    }

    Game::fill_dungeon(world);
    Game::atlas->push_back(world);
    //_CrtMemDumpAllObjectsSince( NULL );

    return world;
}

//creates a person and places them on the current map
Person * Game::create_person(std::string name, int age, int x, int y, char repr, Map* map)
{
    //build the Person
    Person * new_pers = new Person(name, age, x, y, repr);

    //put it on the map somewhere
    Tile * next_tile = map->getTileAt(x,y);
    new_pers->put_person(next_tile, x, y);

    return new_pers;
};

//creates a person and places them on the current map
Person * Game::create_townsmen(std::string name, int age, int x, int y, char repr, 
        Map* map)
{
    //build the Person
    Person * new_pers = new Person(name, age, x, y, repr);

    //put it on the map somewhere
    Tile * next_tile = map->getTileAt(x,y);
    new_pers->put_person(next_tile, x, y);

    new_pers->thinker->is_dumb = true;
    new_pers->thinker->civilian = new Civilian;

    new_pers->is_fighter = false;
    delete new_pers->combat;
    new_pers->combat = NULL;

    return new_pers;
};

    template<class T>
T* Game::create_creature(std::string name, int age, int x, int y, char repr, Map* world)
{
    //build the Person
    T * creature = new T(name, age, x, y, repr);
    //creature->attrs->hunger->max_val = 9999999;

    //put it on the map somewhere
    Tile * next_tile = world->getTileAt(x,y);
    creature->put_person(next_tile, x, y);

    return creature;
};

void give_player_debug_items(Actor* player)
{
    for (int i = 0; i < 52; i++)
    {
        Item* item2 = new Item();
        item2->name = "Item of Holding";
        item2->name.append(std::to_string((long double)i+1));
        item2->attr_effect->health_current_val = 1;
        item2->attr_effect->mana_current_val = 1;
        item2->attr_effect->armor_current_val = 1;
        item2->attr_effect->damage_current_val = 900;
        item2->attr_effect->damage_max_val = 900;
        item2->attr_effect->duration= 4;
        item2->usable = true;
        item2->description = "You can hold this item";
        player->inventory->add_item(item2);
    };
};

//creates a bunch of items on the map
void  Game::initialize_items()
{

    //put it on the map somewhere
    // Tile * next_tile = current_map->getTileAt(1,1);

    // Item* item = new Item();
    // next_tile->place_item_down(item);
    // items.push_back(item);

    Item* dagger = new Item();
    dagger->name = "Prisoner's Shank";
    dagger->repr->repr = '/';
    dagger->description = "You hid this in your clothes";
    dagger->attr_effect->set_all_vals_to(0);
    dagger->attr_effect->damage_current_val = 1;
    dagger->attr_effect->damage_max_val = 1;
    dagger->slot_type= MainHand;
    dagger->equippable = true;
    player->inventory->add_item(dagger);
    player->equipment->equip_item(dagger);

    Item* amulet = new Item();
    amulet->name = "Bent Amulet";
    amulet->repr->repr = '&';
    amulet->description = "You swore an oath to wear this forever.";
    amulet->attr_effect->set_all_vals_to(0);
    amulet->attr_effect->health_current_val = 3;
    amulet->attr_effect->health_max_val = 3;
    amulet->slot_type = Neck;
    amulet->equippable = true;

    Spell* fireball = new FireBallSpell();
    amulet->spell_effect = fireball;
    fireball->master = player;
    // amulet->spell_effect = new TeleportSelfSpell;
    // amulet->spell_effect = new LaunchOtherSpell;
    // amulet->spell_effect->master = player;
    Game::custom_key1->assign_item(amulet);

    player->inventory->add_item(amulet);
    player->equipment->equip_item(amulet);

    Item* chest_armor = new Item();
    chest_armor->name = "Prisoner's Robes";
    chest_armor->repr->repr = '&';
    chest_armor->description = "This is all you were allowed to wear";
    chest_armor->attr_effect->set_all_vals_to(0);
    chest_armor->attr_effect->armor_current_val = 1;
    chest_armor->attr_effect->armor_max_val = 1;
    chest_armor->slot_type = Chest;
    chest_armor->equippable = true;
    player->inventory->add_item(chest_armor);
    player->equipment->equip_item(chest_armor);

    // give_player_debug_items(player);

};

// //creates a bunch of enemies on the map
// void  Game::initialize_enemies()
// {
//     //Game::current_map->enemies.push_back(Game::create_person("First", 99, 20, 2, 'p', Game::current_map, "First Person"));
//     //Game::current_map->enemies.push_back(Game::create_troll("Second", 66, 4, 9, 'T', Game::current_map, "Second, Troll"));
//     //Game::current_map->enemies.push_back(Game::create_skeleton("Third", 33, 14, 9, 's', Game::current_map, "Third, Skeleton"));
// };

void give_player_teleport(Actor* player)
{
    TeleportSelfSpell* teleport = new TeleportSelfSpell();
    teleport->master = player;
    teleport->max_range = 999;
    teleport->min_range = 999;
    teleport->mana_cost = 0;
    player->spells->push_back(teleport);
    Game::custom_key2->assign_spell(teleport);

};

void give_player_god_mode()
{
    Person* player = Game::player;
    player->attrs->health->current_val=3000;
    player->attrs->health->max_val=3000;

    player->attrs->mana->current_val=3000;
    player->attrs->mana->max_val=3000;
    player->attrs->mana->regen_rate=100;
    player->attrs->mana->regen_interval=500;

    player->attrs->damage->current_val=500;
    player->attrs->damage->max_val=500;

    player->attrs->armor->current_val=0;
    player->attrs->armor->max_val=0;

    player->attrs->hunger->max_val=1000;
    player->attrs->hunger->current_val=210;
    player->attrs->hunger->regen_interval=5;

};

Person*  Game::initialize_player()
{
    player = new Person("Player", 23, 0, 0, '@');

    // player->attrs->health->current_val=200;
    // player->attrs->health->max_val=200;
    player->attrs->health->current_val=30;
    player->attrs->health->max_val=30;

    player->attrs->mana->current_val=30;
    player->attrs->mana->max_val=30;
    player->attrs->mana->regen_rate=1;
    player->attrs->mana->regen_interval=5;

    player->attrs->damage->current_val=5;
    player->attrs->damage->max_val=5;

    player->attrs->armor->current_val=0;
    player->attrs->armor->max_val=0;

    player->attrs->hunger->max_val=1000;
    player->attrs->hunger->current_val=210;
    player->attrs->hunger->regen_interval=5;

	//LPWSTR name = "asdasd";
    ClassType chosen_type = Parser().get_preferred_class_type();
    std::vector<IClass*>* choices = Actor::actor_class_choices;
    for (std::vector<IClass*>::iterator it = choices->begin(); it != choices->end(); it++)
    {
		IClass* cls = *it;
        if (cls->type == chosen_type)
            player->actor_class = cls;
    };
    player->representation->setFGColor(player->actor_class->fg_color, true, true, true);
    // player->actor_class = new FighterClass;
    player->actor_class->master = player;

    delete player->thinker;
    player->thinker = NULL;

    //Tile* next_tile = Game::current_map->getTileAt(player->x, player->y);
    Room* room = Game::current_map->roomVector->front();
    int x = room->center_x;
    int y = room->center_y;
    Game::player->put_person(Game::current_map->getTileAt(x, y), x, y);
    //player->put_person(next_tile, player->x, player->y);
    Game::initialize_items();

    // give_player_god_mode();
    // give_player_teleport(player);

    Game::center_camera_on_player();

    return player;

};

void Game::center_camera_on(int abs_x, int abs_y)
{
    Game::camera_x = abs_x - (Game::game_console->getWidth()/2);
    Game::camera_y = abs_y - (Game::game_console->getHeight()/2);
};

void Game::center_camera_on(Actor* actor)
{
    Game::center_camera_on(actor->x, actor->y);
};

void Game::center_camera_on(Tile* tile)
{
    Game::center_camera_on(tile->tile_x, tile->tile_y);
};

void Game::center_camera_on_player()
{
    Game::center_camera_on(Game::player);
};

void Game::update()
{
    //update player
    Game::player->update();

    //update actors in this floor
    int enemies_size = Game::current_map->enemies.size();
    for (actor_vec_t::size_type i = 0; i != enemies_size; i++) 
    {
        Actor* enemy = Game::current_map->enemies.at(i);
        // cout << "\t" << enemy->name << "is updating" << endl;
        if (enemy->is_active && enemy->thinker != NULL)
        {
            enemy->update();
            // printf("updating ai\n");
        };
        // printf("updating\n");
    }
    //update actors in this floor
    int allies_size = Game::current_map->allies.size();
    for (actor_vec_t::size_type i = 0; i != allies_size; i++) 
    {
        Actor* ally = Game::current_map->allies.at(i);
        // cout << "\t" << ally->name << "is updating" << endl;
        if (ally->is_active && ally->thinker != NULL)
        {
            ally->update();
            // printf("updating ai\n");
        };
        // printf("updating\n");
    }

    Game::current_map->update();
    // cout << "\t" << "done updating" << endl;
};

void Game::update_ui()
{
    switch(Game::current_state)
    {
        case GameStates::GameplayState: 
            Ui::update_ui();
            break;
            // case GameStates::MenuState:
            //     Ui::update_inventory_ui();
            //     break;
    };
}

void Game::draw_ui()
{
    if (Game::current_state == GameStates::GameplayState)
        // switch(Game::current_state)
    {
        // case GameStates::GameplayState: 
        Ui::draw_ui();
        // break;
    }
    else
    {
        switch(Game::current_screen)
        {
            case Screens::InventoryScreenType:
                Ui::draw_inventory_main();
                break;
            case Screens::MainMenuScreenType:
                Ui::draw_main_menu_ui();
                break;
            case Screens::CharacterSheetScreenType:
                Ui::draw_char_sheet_ui();
                break;
            case Screens::HelpScreenType:
                Ui::draw_help_ui();
                break;
            case Screens::SpellSelectScreenType:
                Ui::draw_spell_select_main();
                break;
            case Screens::ClassSelectScreenType:
                Ui::draw_class_select_main();
                break;
            default:
                assert(false && "Unknown Screens");
                break;
        };
        TCODConsole::root->setDirty(0, 0, 1000, 1000);
    };
};

bool gameplay_loop(bool incr_turn)
{
    if (incr_turn)
    {

        //this used to be after input was processed but turn hadn't
        //been incremented
        int item_count = Game::player->my_tile->inventory->get_count();
        if (item_count == 1)
        {
            std::string msg_str =  "%s is on the ground.";
            new Message(Ui::msg_handler_main, ITEM_MSG, msg_str, Game::player->my_tile->inventory->items->back()->name.c_str());
        }
        else if (item_count > 1)
        {
            std::string msg_str = "%d items are on the ground.";
            new Message(Ui::msg_handler_main, ITEM_MSG, msg_str, item_count);
        }

        //tile description
        new Message(Ui::msg_handler_main, TILE_DESCRIPTION_MSG, "%s", Game::player->my_tile->get_description().c_str());

        //new Message(Ui::msg_handler_main, NOTYPE_MSG, "TURN: %d", Game::turn_count);
        Game::turn_count++;
        printf("\n-------------[ TURN: %d ]-------------\n", Game::turn_count);
        incr_turn = false;
    }

    // TCOD_event_t evt = TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key_evt, &mouse_evt, false);
    if ((Game::key_evt.vk != NULL || Game::key_evt.c != NULL) && Game::key_evt.pressed == 1 ){
        incr_turn = Input::process_key_event(Game::key_evt);
    }
    if (Game::key_evt.pressed == 1)
    {
        Input::process_debug_event(Game::key_evt);
    }

    if (Input::process_mouse_event(Game::mouse_evt))
    {
        incr_turn = true;
    };

    //AIs update
    if (incr_turn == true)
    {
        Game::update();
    }

    Game::update_ui();

    //draw the map to libtconsole
    Game::current_map->draw();

    //draw the UI
    Game::draw_ui();






    //draw the game_console to root, taking from where the camera is looking at
    TCODConsole::root->blit(Game::game_console, Game::camera_x, Game::camera_y,
            Game::camera_w, Game::camera_h, TCODConsole::root, 0, 0);
    Game::game_console->clear();

    return incr_turn;
};

void set_icon()
{

    SetConsoleTitle(Game::term_name.c_str());
    HWND hwnd2 = FindWindow(NULL, Game::term_name.c_str());
    std::string s2 = std::string(get_data_path()+std::string("img\\favicon.ico")).c_str();
    std::wstring stemp2 = std::wstring(s2.begin(), s2.end());
    LPCWSTR sw2 = stemp2.c_str();
    HANDLE icon2 = LoadImage(NULL, sw2, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    SendMessage(hwnd2, (UINT)WM_SETICON, ICON_BIG, (LPARAM)icon2);

    HWND hwnd = FindWindow(NULL, Game::main_name.c_str());
    std::string s = std::string(get_data_path()+std::string("img\\favicon.ico")).c_str();
    std::wstring stemp = std::wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    HANDLE icon = LoadImage(NULL, sw, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    SendMessage(hwnd, (UINT)WM_SETICON, ICON_BIG, (LPARAM)icon);

}

void Game::init_custom_keys()
{
    Game::custom_keys = new ckey_vec_t();
    Game::custom_keys->push_back(Game::custom_key1);
    Game::custom_keys->push_back(Game::custom_key2);
    Game::custom_keys->push_back(Game::custom_key3);
    Game::custom_keys->push_back(Game::custom_key4);
    Game::custom_keys->push_back(Game::custom_key5);
};

CustomKey* Game::get_free_custom_key()
{
    for (ckey_vec_t::iterator it = Game::custom_keys->begin();
            it != Game::custom_keys->end();
            it++)
    {
        CustomKey* ck = *it;
        if (ck->element == NULL)
        {
            return ck;
        }
    };

    return NULL;
};

void Game::init_rng()
{
    Parser parser = Parser();
    if (parser.get_spawning_rng_seed() != NULL) 
    {
        delete Game::spawning_rng;
        Game::spawning_rng = new TCODRandom(parser.get_spawning_rng_seed());
    };
    Game::spawning_rng->setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);

    if (parser.get_item_spawn_rng_seed() != NULL) 
    {
        delete Game::item_spawn_rng;
        Game::item_spawn_rng = new TCODRandom(parser.get_item_spawn_rng_seed());
    };
    Game::item_spawn_rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);

    if (parser.get_linear_rng_seed() != NULL) 
    {
        delete Game::linear_rng;
        Game::linear_rng = new TCODRandom(parser.get_linear_rng_seed());
    };
    Game::linear_rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);

    if (parser.get_event_rng_seed() != NULL) 
    {
        delete Game::event_rng;
        Game::event_rng = new TCODRandom(parser.get_event_rng_seed());
    };
    Game::event_rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);

    if (parser.get_stat_rolls_rng_seed() != NULL) 
    {
        delete Game::stat_rolls_rng;
        Game::stat_rolls_rng = new TCODRandom(parser.get_stat_rolls_rng_seed());
    };
    Game::stat_rolls_rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);

    if (parser.get_dungeon_builder_rng_seed() != NULL) 
    {
        delete Game::dungeon_builder_rng;
        Game::dungeon_builder_rng = new TCODRandom(parser.get_dungeon_builder_rng_seed());
    };
    Game::dungeon_builder_rng->setDistribution(TCOD_DISTRIBUTION_LINEAR);
};

void Game::start_game()
{
    printf("YOU ARE PLAYING: BiochRL++ %s\n", Game::get_version().c_str());

    Game::init_engine();

    Game::init_custom_keys();

    //TODO use ClassType instead of actual class instances like an idiot
    Actor::actor_class_choices->push_back(new FighterClass);
    Actor::actor_class_choices->push_back(new StalkerClass);
    Actor::actor_class_choices->push_back(new MageClass);
    Actor::actor_class_choices->push_back(new BrawlerClass);
    Actor::actor_class_choices->push_back(new NecromancerClass);

    Game::init_rng();


    std::cout << "starting world gen" << std::endl;
    // Map* new_map = Game::build_world(1);
    Map* new_map = Game::build_town();
    std::cout << "ending world gen" << std::endl;
    Game::current_map = new_map;

    Game::initialize_player(); //created the Person player
    Game::mainloop();


};

void Game::init_engine()
{
    TCODConsole::setCustomFont("data/terminal.png");
    TCODConsole::initRoot(screen_w, screen_h, std::string(Game::main_name.begin(), Game::main_name.end()).c_str(), false);

    Game::fps_limit = 60;
    TCODSystem::setFps(fps_limit);
    TCODConsole::setKeyboardRepeat(1000, 1);

    set_icon();
};

void play_music()
{
    /* straight copied from
     * http://stackoverflow.com/questions/17472514/no-sound-with-sdl-mixer
     */

    Mix_Music *music = NULL;
    Mix_Chunk *wave = NULL;

    SDL_Init(SDL_INIT_AUDIO);

    int audio_rate = 48000;
    Uint16 audio_format = AUDIO_S32SYS; /* 16-bit stereo */
    int audio_channels = 1;
    int audio_buffers = 4096;

    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
        printf("Unable to open audio!\n");
        exit(1);
    }

    // if(Mix_Init(MIX_INIT_MOD) != MIX_INIT_MOD)
    //     std::cout << "MIX_INIT_MOD isn't the same";

    // Mix_Volume(-1, MIX_MAX_VOLUME);

    music = Mix_LoadMUS(std::string(get_data_path()+"lvl1.wav").c_str());

    if (music == NULL) {
        std::cout << "Could not load 1.wav\n";
        std::cout << Mix_GetError();
    }


    std::cout << std::endl;
    std::cout << Mix_GetError();
    std::cout << std::endl;

    //parser to read file settings. currently only music
	Parser* parser = new Parser();

    bool should_enable = parser->get_enable_music();
    if (should_enable)
    {
        Mix_FadeInMusic(music, -1, 1000);
        float percent = parser->get_music_volume();
        float volume = MIX_MAX_VOLUME*percent;
        Mix_VolumeMusic(volume);
    }

    delete parser;
};

void Game::mainloop()
{

    WelcomeMessage();
    //move main window over a bit so that the console isn't blocked
    move_window(600, 100);

    bool incr_turn  = false;
    Game::turn_count = 1;


    // current_map->draw();
    //TCODConsole::setFullscreen(true);
    // auto renderer = TCODSystem::getRenderer();
    // TCODSystem::setRenderer(TCOD_renderer_t::TCOD_RENDERER_OPENGL); //risky
    // TCODSystem::setRenderer(TCOD_renderer_t::TCOD_RENDERER_GLSL); // really fast if you can man it
    TCODConsole::setKeyboardRepeat(250, 10);

    new Message(Ui::msg_handler_main, NOTYPE_MSG, "Run from ogres and skeletons, kill troll and jackals!");
    new Message(Ui::msg_handler_main, NOTYPE_MSG, "You're going to die a lot, be careful.");
    //

    //draw the map to libtconsole
    current_map->draw();
    //draw the UI
    Game::draw_ui();
    Game::current_state = MenuState;
    Game::current_screen = ClassSelectScreenType;

    //draw libtcon to screen
    TCODConsole::flush();

    //music
    play_music();

    assert(Game::current_map != NULL);
    while ( !TCODConsole::isWindowClosed() ) 
    {

        TCODSystem::checkForEvent(TCOD_EVENT_ANY, &key_evt, &mouse_evt);
        switch(Game::current_state)
        {
            case GameStates::GameplayState: 
                incr_turn = gameplay_loop(incr_turn);
                break;

            case GameStates::MenuState:
                if ((key_evt.vk != NULL || key_evt.c != NULL) && key_evt.pressed == 1 ){
                    incr_turn = Input::process_key_event(key_evt);
                };

                Game::draw_ui();
                Input::process_mouse_inv_event();
                break;

            default:
                assert(false && "Unknown gamestate");
                break;
        }

        //draw the root console to screen to screen
        // std::stringstream ss;
        // std::string msg_str;
        // msg_str = "%c%c%c%cred test%c%c%c%c%cgreen text now%c";
        // char buffer[999];
        // sprintf(buffer, msg_str.c_str(),  TCOD_COLCTRL_FORE_RGB, 255, 1, 1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_FORE_RGB, 255, 255, 1, TCOD_COLCTRL_STOP);
        // ss << buffer;
        // TCODConsole::root->print(0, 0, ss.str().c_str());
        // TCODConsole::root->print(0, 0, buffer);
        TCODConsole::flush();
        TCODConsole::root->clear();

        // assert(Game::tick_count!= -1);
        Game::tick_count++;

        //when the game is moved, the screen goes black for some reason. This
        //redraws the whole screen to make up for it. f8 does it manually.
        if (Game::tick_count % 50 == 0)
            TCODConsole::root->setDirty(0, 0, 1000, 1000);
    }

    std::cout << "Hit enter to exit" << std::endl;
};
