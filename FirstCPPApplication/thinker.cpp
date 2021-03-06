#include "stdafx.h"
#include "thinker.h"

#include <random>
#include "assert.h"

#include "actors\actor.h"
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
#include <messages.h>
#include "ui.h"

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
    this->tracking_memory = 10;

    this->is_ally = false;
    this->target = NULL;

    this->_is_aware = false;
};

Thinker::Thinker(Thinker& other)
{
    this->master = other.master;
    this->civilian = other.civilian;

    this->consecutive_fails = other.consecutive_fails;
    this->fail_threshold = other.fail_threshold;
    this->path_possible = other.path_possible;
    this->skipped_tries = other.skipped_tries;
    this->consecutive_skips = other.consecutive_skips;
    this->skip_threshold = other.skip_threshold;

    this->is_dumb = other.is_dumb;
    this->is_civ = other.is_civ;
    this->is_shopkeep = other.is_shopkeep;

    this->turn_last_seen_by_player = other.turn_last_seen_by_player;
    this->tracking_memory = other.tracking_memory;

    this->is_ally = other.is_ally;
    this->target = other.target;

    this->_is_aware = other._is_aware;
};

bool Thinker::is_aware(Actor* actor)
{
    //if player not sneaking > is aware
    //if aware > is aware
    //if player sneaking and aware -> is aware
    //if player sneaking -> is not aware

    if (this->_is_aware) 
    {
        return true;
    }

    //try to detect
    this->_is_aware = this->master->try_detect(actor);
    if (this->_is_aware && actor->is_sneaking)
    {
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkRed, "Detected %s!"), actor->name.c_str());
    };

    return this->_is_aware;
};

bool Thinker::get_is_aware()
{
    return this->_is_aware;
};

Thinker::~Thinker()
{
    if (this->master != NULL && this->master->l_path != NULL)
    {
        delete this->master->l_path;
    };

};

void Thinker::update_path()
{ 
    Map* map = Game::player->my_tile->map;
    if (this->master->name == "Skellie")
    {
        int temp = 1+1;
    }
    //get the destination tile coords

    assert(this->target != NULL && "thinkers need a target to move towards, usually the player");
    if (this->target->is_active)
    {
        Tile* target_tile = this->target->my_tile;

        //if the target tile is adjacent to the player keep moving towards
        //it, otherwise change spots
        int dest_tile_x, dest_tile_y;
        master->l_path->getDestination(&dest_tile_x, &dest_tile_y);
        tile_vec_t* adj_tiles = Game::current_map->getTileAt(dest_tile_x, dest_tile_y)->getAdjacentTiles();
        tile_vec_t::iterator adjItr = std::find(adj_tiles->begin(), adj_tiles->end(), target_tile);
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

        };
        delete adj_tiles;
    }
    else
    {
        delete master->l_path;
        master->l_path = NULL;
    }




};

void Thinker::set_target()
{
    if (this->is_ally) 
    {
        //TODO select monster near tile
        if (!Game::player->actors_in_sight->empty())
        {
            actor_vec_t::iterator potential_target = Game::player->actors_in_sight->begin();
            for (potential_target; potential_target!=Game::player->actors_in_sight->end(); potential_target++)
            {
                if ((*potential_target)->combat != NULL && !(*potential_target)->thinker->is_ally)
                {
                    this->target = *potential_target;
                    break;
                }
                this->target = Game::player;
            }
        }
        else
        {
            this->target = Game::player;
        }
    }
    else 
    {
        this->target = Game::player;
    };

};

void Thinker::set_destination()
{
    tile_vec_t* adjacent_tiles;
    adjacent_tiles  = this->target->my_tile->getVacantAdjacentTiles();  
    std::random_shuffle(adjacent_tiles->begin(), adjacent_tiles->end());

    if (adjacent_tiles->size() > 0)
    {
        master->dest_x = adjacent_tiles->back()->tile_x;
        master->dest_y = adjacent_tiles->back()->tile_y;
    }
    delete adjacent_tiles;

};

void Thinker::build_path()
{
    Map* map = Game::player->my_tile->map;
    // std::cout << "Building Path" << std::endl;
    master->l_path = new TCODPath(map->l_map);

    //set the master's target to player or enemy, depending if ally or not
    this->set_target();
    this->set_destination();

    int x = master->x;
    int y = master->y;
    int dest_x = master->dest_x;
    int dest_y = master->dest_y;

    //compute a path for the master to walk
    this->path_possible = master->l_path->compute(x, y, dest_x, dest_y);

    //if there's a not a path to be walked, incr fail count
    if (!this->path_possible)
    {
        this->consecutive_fails+=1;
    }
    else
    {
        this->consecutive_fails=0;
    }

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
        this->update_path();
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
            this->build_path();
        }
    };

    if (this->path_possible && this->master->l_path != NULL)
    {
        this->walk_towards_target();
        bool path_empty = master->l_path->isEmpty();
        if (path_empty)
        {
            this->try_attacking_player();
        }
        else
        {
            if (this->master->is_champion && this->master->champion_type==Healer)
            {
                actor_vec_t targets = this->master->my_tile->getActorsAroundTile(2);
                for (actor_vec_t_it it = targets.begin(); it!=targets.end(); it++)
                {
                    Actor* target = *it;
                    if (target != Game::player) //note will heal player allies
                    {
                        target->attrs->health->current_val+=5;
                    };
                    new Message(Ui::msg_handler_main, HELP_MSG, "%s healed %s!", this->master->name.c_str(), target->name.c_str());
                };
                printf("wololo");
            }
        };
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
    tile_vec_t* adj_tiles = this->master->my_tile->getVacantAdjacentTiles();
    if (! adj_tiles->empty())
    {
        std::random_shuffle ( adj_tiles->begin(), adj_tiles->end());
        Tile * next_tile = adj_tiles->back();
        master->put_person(next_tile, next_tile->tile_x, next_tile->tile_y); 
    };
    delete adj_tiles;
};

void Thinker::walk_towards_target()
{
    assert(this->target != NULL && "thinkers need a target to move towards, usually the player");
    master->l_path->walk(&master->x, &master->y, true);
    Tile * next_tile = this->target->my_tile->map->getTileAt(master->x, master->y);
    master->put_person(next_tile, master->x, master->y); 
}

void Thinker::try_attacking_player()
{
    if (this->is_ally && this->target == Game::player)
    {
        if (this->master->combat->get_last_attacker() == Game::player->combat)
        {
            new Message(Ui::msg_handler_main, HELP_MSG, "Your ally cries 'Forgive me for this, master.'");
        }
        else
        {
            return;
        }
    }
    assert(this->target != NULL && "thinkers need a target to move towards, usually the player");
    //attack the player if he's in range (aka adjacent tile)
    Combat* assailant = this->target->combat;
    tile_vec_t* adjacent_tiles = ((Person*)master)->my_tile->getAdjacentTiles();
    if (std::find(adjacent_tiles->begin(), adjacent_tiles->end(), assailant->master->my_tile) != adjacent_tiles->end())
    {
        // std::cout << "adjacent" << std::endl;
        Damage* dmg= master->attrs->get_effective_damage(); //TODO use all damage types instead of just normal
        ((Person*)master)->combat->attack(assailant, dmg);
        delete dmg;
    };

    //calm the combat down
    ((Person*)master)->combat->was_attacked = false;

}

void Thinker::set_aware(bool aware)
{
    this->_is_aware = aware;
};

void Thinker::update()
{
    if (Game::player->is_actor_in_sight(this->master)) //when thinker becomes truly activated or whatever
    {
        this->turn_last_seen_by_player = Game::turn_count;
        //    std::cout << "START: " << this->turn_last_seen_by_player << " END."  << std::endl;
        Person* player = Game::player;
        int distance_between_player = get_euclidean_distance(this->master->x, this->master->y, player->x, player->y);
        bool player_within_distance = distance_between_player < Thinker::visibility_threshold;

        if (!this->is_dumb && player_within_distance)
        {
            if (!this->is_aware(Game::player)) //thinker is not aware of player yet, via sneaking or w/e
            {
                this->dumb_update();
            }
            else
            {
                //aka pathing and fighting
                this->smart_update();
            };
        }
        else
        {
            //walking to adjacent tile randomly
            this->dumb_update();
        }
    }
    else 
    {
        int turns_since_last_seen = Game::turn_count - this->turn_last_seen_by_player;
        if (turns_since_last_seen < this->tracking_memory)
        {
            auto player = Game::player;
            int distance_between_player = get_euclidean_distance(this->master->x, this->master->y, player->x, player->y);
            if (!this->is_dumb && distance_between_player < Thinker::visibility_threshold && this->get_is_aware())
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
    };
};
