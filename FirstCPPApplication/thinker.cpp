#include "stdafx.h"
#include "thinker.h"

#include <random>

#include "libtcod.hpp"
#include "map.h"
#include <actors\Person.h>
#include "game.h"
#include "Representation.h"
#include "tile.h"	
#include "combat.h"
#include "attribute.h"
#include "attribute_container.h"
#include "utils.h"

int Thinker::visibility_threshold = 50;

Thinker::Thinker()
{
    this->master = NULL;
    this->civilian = NULL;

    this->consecutive_fails = 0;
    this->fail_threshold = 5;
    this->path_possible = true;
    this->skipped_tries = 0;
    this->consecutive_skips = 0;
    this->skip_threshold = 0;

    this->is_dumb = false;
    this->is_civ = false;
    this->is_shopkeep = false;

    this->turn_last_seen_by_player = -999;
    this->tracking_memory = 25;
};

Thinker::~Thinker()
{
    delete this->master->l_path;

};

void Thinker::smart_update()
{
    // printf("thinking\n");
    //PATH FINDING
    Map* map = Game::player->my_tile->map;

    //in order for the AI to keep looking for the player, the path needs to
    //be rebuilt every update, otherwise he just walks to the same place
    //until he gets there
    if (master->l_path != NULL)
    {
        //get the destination tile coords
        int dest_tile_x, dest_tile_y;
        master->l_path->getDestination(&dest_tile_x, &dest_tile_y);

        Tile* player_tile = Game::player->my_tile;

        //if the target tile is adjacent to the player keep moving towards
        //it, otherwise change spots
        std::vector<Tile*>* adj_tiles = Game::world->getTileAt(dest_tile_x, dest_tile_y)->getAdjacentTiles();
        std::vector<Tile*>::iterator adjItr = std::find(adj_tiles->begin(), adj_tiles->end(), player_tile);
        //if the path destination isn't adj to the player make a new path 
        if (adjItr == adj_tiles->end())
        {


            // std::cout << "no tiles adjacent to player where I'm pathing to, so I'm making a new path" << std::endl;
            delete master->l_path;
            master->l_path = NULL;
        }
        else
        {
            //continue on that path

            //std::cout << "dest x: " << dest_tile_x;
            //std::cout << "dest y: " << dest_tile_y << std::endl;
            //std::cout << "CONTINUTE" << std::endl;

        };
        delete adj_tiles;


    };

    if (master->l_path == NULL)
    {
        if (this->consecutive_fails > this->fail_threshold)
        {
            if (this->consecutive_skips < this->skip_threshold)
            {
                this->consecutive_skips += 1;
            }
            else
            {
                this->path_possible = true;
                this->consecutive_fails = 0;
            }
        }
        else
        {
            // std::cout << "Building Path" << std::endl;
            master->l_path = new TCODPath(map->l_map);

            //set the master's destination to above the player
            std::vector<Tile*>* adjacent_tiles = Game::player->my_tile->getVacantAdjacentTiles();
            std::random_shuffle(adjacent_tiles->begin(), adjacent_tiles->end());
            if (adjacent_tiles->size() >0)
            {
                master->dest_x = adjacent_tiles->back()->tile_x;
                master->dest_y = adjacent_tiles->back()->tile_y;
            }
            delete adjacent_tiles;

            int x, y, dest_x, dest_y;
            x = master->x;
            y = master->y;
            dest_x = master->dest_x;
            dest_y = master->dest_y;

            //compute a path for the master to walk
            this->path_possible = master->l_path->compute(x, y, dest_x, dest_y);
            if (!this->path_possible)
            {
                // std::cout << "path aint possible" << std::endl;
                this->consecutive_fails+=1;
            }
            else
            {
                this->consecutive_fails=0;
            }
        }

        //int path_size = master->l_path->size();
        // cout << "Path size AFTER BUILDING IT: " << path_size << endl << "I'mna walk it" << endl;
    };

    // int *xx = 0, *yy = 0;
    // master->l_path->getDestination(xx, yy);
    // std::cout << xx << " x" ;
    // std::cout << &xx << " &x" ;
    //std::cout << *xx << " *x" ;
    // std::cout << yy << " y" << std::endl;
    if (this->path_possible && this->master->l_path != NULL)
    {
        this->walk_towards_player();

        bool path_empty = master->l_path->isEmpty();
        // cout << "Path size: " << path_size << endl << "I'mna walk it" << endl;
        if (path_empty)
        {
            this->try_attacking_player();
        }
    }

};

void Thinker::dumb_update()
{
    if (Game::turn_count % 3 == 0)
    {
        this->walk_dumbly();
    }
};


void Thinker::walk_dumbly()
{
    //pick a random direction if its not a wall or a boundary of some kind ie
    //the exit of their shop
    std::vector<Tile*>* adj_tiles = this->master->my_tile->getVacantAdjacentTiles();
    std::random_shuffle ( adj_tiles->begin(), adj_tiles->end());
    Tile * next_tile = adj_tiles->back();
    master->putPerson(next_tile, next_tile->tile_x, next_tile->tile_y); 
    delete adj_tiles;
};

void Thinker::walk_towards_player()
{
    master->l_path->walk(&master->x, &master->y, true);
    Tile * next_tile = Game::player->my_tile->map->getTileAt(master->x,master->y);
    master->putPerson(next_tile, master->x, master->y); 
}

void Thinker::try_attacking_player()
{
    // cout << "IMNA ATTACK THE PLAYER" << endl;
    //attack the player if he's in range (aka adjacent tile)
    Combat* assailant = Game::player->combat;
    std::vector<Tile*>* adjacent_tiles = ((Person*)master)->my_tile->getAdjacentTiles();
    if (std::find(adjacent_tiles->begin(), adjacent_tiles->end(),
                assailant->master->my_tile) != adjacent_tiles->end())
    {
        // std::cout << "adjacent" << std::endl;
        ((Person*)master)->combat->Attack(assailant, master->attrs->damage->current_val);
    };

    //calm the combat down
    ((Person*)master)->combat->was_attacked = false;

};

void Thinker::update()
{
    // return;

    // vector<Actor*>::iterator aisItr;
    // aisItr = std::find(Game::player->actors_in_sight->begin(), Game::player->actors_in_sight->end(),  this->master);
    if (Game::player->IsActorInSight(this->master))
    {
        //this->turn_last_seen_by_player = Game::turn_count;
        //    std::cout << "START: " << this->turn_last_seen_by_player << " END."  << std::endl;
        auto player = Game::player;
        int distance_between_player = get_euclidean_distance(this->master->x, this->master->y, player->x, player->y);
        if (!this->is_dumb && distance_between_player < Thinker::visibility_threshold)
        {
            //aka pathing and fighting
            this->smart_update();
        }
        else
        {
            //walking adjacent
            this->dumb_update();
        }
    }
    else 
    {
            //std::cout << this->turn_last_seen_by_player << std::endl;
        if ((Game::turn_count - this->turn_last_seen_by_player) < this->tracking_memory)
        {
            // std::cout << "i remember" << std::endl;
            auto player = Game::player;
            int distance_between_player = get_euclidean_distance(this->master->x, this->master->y, player->x, player->y);
            if (!this->is_dumb && distance_between_player < Thinker::visibility_threshold)
            {
                //aka pathing and fighting
                this->smart_update();
            }
            else
            {
                //walking adjacent
                this->dumb_update();
            }

        }
        else 
        {
            // std::cout << "i forget" << std::endl;
        };

    };

};
