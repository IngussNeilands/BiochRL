#include "stdafx.h"
#include <map_builder.h>

#include "map.h"
#include "room.h"
#include "game.h"
#include "actors/Person.h"
#include <randsys.h>

#include "enemies\troll.h"
#include "enemies\jackal.h"
#include "enemies\skeleton.h"
#include "enemies\ogre.h"
#include <enemies\bad_mother.h>
#include <enemies\hulkingmantis.h>
#include <enemies\idol.h>
#include <enemies\imp.h>
#include <enemies\mutantfish.h>
#include <enemies\lizard.h>
#include <enemies\crazedcook.h>
#include <enemies\wildling.h>
#include <enemies\sludge.h>
#include <enemies\jumper.h>
#include <tile.h>
#include <drop_handler.h>
#include <Representation.h>
#include <item.h>


void MapBuilder::fill_dungeon(Map* world)
{
    //fill rooms with enemies and monsters
    std::vector<Room*>* rooms = world->roomVector;
    for (std::vector<Room*>::iterator it = rooms->begin(); it != rooms->end(); ++it)
    {
        if (it == rooms->begin()+4)
        {
            //spawn one dude to whom you can sell your shit
            Room* room = *it;
            int x = room->center_x;
            int y = room->center_y;
            Person* the_townsmen = Game::create_townsmen("Travelling Salesman", 30, x, y, 't', world);
            world->allies.push_back(the_townsmen);

        }
        else if (it == rooms->begin()+6)
        {
            Room* room = *it;
            int x = room->center_x;
            int y = room->center_y;

            Tile* chest_tile = world->getTileAt(x+1, y+1);
            chest_tile->updateTileType(tile_type_h::ChestTileTypeType);

            Item* item;
            for (int i = 0; i < 3; i++)
            {
                item = DropHandler::handle_for_chest();
                if (item != NULL)
                {
                    chest_tile->place_item_down(item);
                }
            };

        }
        else
        {
            if (it != rooms->begin()) // player needs an empty room to spawn in
            {
                fill_generic_room(*it, world);
            };
        }
    };

    int count = 0;
    for (arr_vec_int_t_it it = world->inside_tiles->begin(); it != world->inside_tiles->end(); it++)
    {
        count++;
        if (count % 300 == 0)
        {
            int x = (*it)[0];
            int y = (*it)[1];
            Tile* tile = world->getTileAt(x, y);
            Representation* repr = new Representation;
            repr->repr = '"';
            repr->setFGColor(TCODColor::darkestGreen, true, true, true);

            Item* plant = new Item();
            plant->usable = true;
            plant->repr = repr;
			plant->attr_effect->health_current_val = 25;
            plant->name = "Strange Green Plant";
            tile->place_item_down(plant);
        };
    };

}

void MapBuilder::fill_generic_room(Room* room, Map* world)
{
    int floor = world->depth;
    // if (Game::current_map == NULL) { floor = 1; }
    // else { floor = Game::current_map->depth; };
    MonsterSpawnTypes spawn_type = get_spawn_type(floor);
    if (spawn_type == MonsterSpawnTypes::TrollSpawn)
    {
        Game::spawn_creature<Troll>(room, "Troll", 34, 'T', world);
    }
    else if (spawn_type == MonsterSpawnTypes::JackalSpawn)
    {
        Game::spawn_creature<Jackal>(room, "Jackal", 31, 'j', world);
    }
    else if (spawn_type == MonsterSpawnTypes::HulkingMantisSpawn)
    {
        Game::spawn_creature<HulkingMantis>(room, "HulkingMantis", 31, 'm', world);
    }
    else if (spawn_type == MonsterSpawnTypes::IdolSpawn)
    {
        Game::spawn_creature<Idol>(room, "Idol", 31, 'i', world);
    }
    else if (spawn_type == MonsterSpawnTypes::ImpSpawn)
    {
        Game::spawn_creature<Imp>(room, "Imp", 31, 'i', world);
    }
    else if (spawn_type == MonsterSpawnTypes::MutantFishSpawn)
    {
        Game::spawn_creature<MutantFish>(room, "MutantFish", 31, 'f', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SpinyLizardSpawn)
    {
        Game::spawn_creature<SpinyLizard>(room, "SpinyLizard", 31, 'l', world);
    }
    else if (spawn_type == MonsterSpawnTypes::CrazedCookSpawn)
    {
        Game::spawn_creature<CrazedCook>(room, "CrazedCook", 31, 'c', world);
    }
    else if (spawn_type == MonsterSpawnTypes::WildlingSpawn)
    {
        Game::spawn_creature<Wildling>(room, "Wildling", 31, 'w', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SludgeSpawn)
    {
        Game::spawn_creature<Sludge>(room, "Sludge", 31, 's', world);
    }
    else if (spawn_type == MonsterSpawnTypes::JumperSpawn)
    {
        Game::spawn_creature<Jumper>(room, "Jumper", 31, 'j', world);
    }
    else if (spawn_type == MonsterSpawnTypes::OgreSpawn)
    {
        Game::spawn_creature<Ogre>(room, "Ogre", 103, 'O', world);
    }
    else if (spawn_type == MonsterSpawnTypes::SkeletonSpawn)
    {
        Game::spawn_creature<Skeleton>(room, "Skeleton", 92, 's', world);
    }
    else if (spawn_type == MonsterSpawnTypes::BadMotherSpawn)
    {
        Game::spawn_creature<BadMother>(room, "BadMother", 92, 'B', world);
    }
};

MonsterSpawnTypes MapBuilder::get_spawn_type(int floor)
{
    RandomWeightMap<MonsterSpawnTypes> rwm = RandomWeightMap<MonsterSpawnTypes>();
    if (floor == 1)
    {
        rwm.add_item(TrollSpawn, 60);
        rwm.add_item(JackalSpawn, 60);
        rwm.add_item(SkeletonSpawn, 24);
        rwm.add_item(IdolSpawn, 2);
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

    return rwm.get_item(Game::spawning_ratio_rng);
};

Map* MapBuilder::build_town()
{
    Map* world = new Map;
    world->build_town_from_random(0);
    this->fill_town(world);

    std::cout << "going in " << std::endl;
    while (!validate_town(world))
    {
        std::cout << "\t\tlooping " << std::endl;
        delete world;
        world = new Map;
        world->build_town_from_random(0);
        this->fill_town(world);
    };
    return world;
};

void MapBuilder::fill_town(Map* world)
{
    Person* the_townsmen = Game::create_townsmen("Random Townsmen", 30, 10, 10, 't', world);
    world->enemies.push_back(the_townsmen);
};

bool MapBuilder::validate_town(Map* town)
{
    // return false;
    if (town == NULL)
    {
        return false;
    };

    Room* first_room = town->roomVector->front();
    //go through rooms trying to path to the first room, and rejecting if can't 
    for (auto it = town->roomVector->begin(); it != town->roomVector->end(); it++)
    {
        Room* room = *it;
        if (room == first_room) { continue; }
        TCODPath path = TCODPath(town->l_map);
        int ox= room->center_x+1;
        int oy = room->center_y+1;
        int dx = first_room->center_x-1;
        int dy = first_room->center_y-1;
        path.compute(ox, oy, dx, dy);
        if (path.isEmpty())
        {
            printf("failed validation from %d, %d to %d, %d", ox, oy, dx, dy);
            return false;
        }
    };

    return true;
};
