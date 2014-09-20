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
        else
        {
            if (it != rooms->begin()) // player needs an empty room to spawn in
            {
                fill_generic_room(*it, world);
            };
        }
    }

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
        rwm.add_item(TrollSpawn, 20);
        rwm.add_item(JackalSpawn, 20);
        rwm.add_item(SkeletonSpawn, 8);
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
