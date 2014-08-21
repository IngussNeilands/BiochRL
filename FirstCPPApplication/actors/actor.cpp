#include "stdafx.h"

#include <fstream>
#include <time.h>

// #include "libtcod_cpp_hpp\libtcod.hpp"

#include "actor.h"
#include "Person.h"

#include "thinker.h"
#include "inventory.h"
#include "equipment.h"
#include "tile.h"
#include "attribute.h"
#include "attribute_container.h"
#include "Representation.h"
#include "item.h"
#include "attr_effect.h"
#include "ui.h"
#include "messages.h"
#include "game.h"
#include "class.h"
#include <enums\spawntypes_t.h>
#include <randsys.h>
#include <utils.h>
#include <spells.h>
#include "map.h"
#include "combat.h"
#include <drop_handler.h>


int Actor::pack_size = 10;
int Actor::champ_chance = 10;
int Actor::preferred_pack_size = 10;

std::vector<IClass*>* Actor::actor_class_choices = new std::vector<IClass*>();

Actor::Actor()
{
    this->cls_name = "Actor";
    this->name = "Unset Actor name";
    this->actor_class = NULL;

    this->img_path = get_data_path()+"img/Troll_Icon_01.png";

    this->target_queue_tick = 0;

    this->xp = 0;
    this->xp_this_level = 0;
    this->xp_required_to_lvlup = 100;
    this->xp_value = 10;
    this->level = 1;

    this->is_champion = false;
    this->is_hero = false;
    this->timed_attr_effects = new std::vector<TimedEffect*>;
    this->timed_spell_effects = new std::vector<TimedEffect*>;

    this->my_tile = NULL;

    this->representation = new Representation;
    this->background = NULL;
    this->inventory = new Inventory();
    this->inventory->master = this;

    this->attrs = new AttributeContainer;
    this->attrs->owner = this;
    this->spells = new spell_vec_t;

    this->actors_in_sight = new actor_vec_t;

    this->thinker = new Thinker;
    this->thinker->master = this;

    this->is_active = true;

    this->is_moving_left = false;
    this->is_moving_right = false;
    this->is_moving_up = false;
    this->is_moving_down = false;
    this->is_sneaking = false;
    this->is_defending = false;

    this->l_path = NULL;
    this->has_attacked = false;
};

Actor::Actor(Actor& other)
{
    this->cls_name = other.cls_name;

    this->name = "Unset Actor name";
    this->actor_class = other.actor_class;

    this->img_path = other.img_path;

    this->target_queue_tick = other.target_queue_tick;

    this->xp = other.xp;
    this->xp_this_level = other.xp_this_level;
    this->xp_required_to_lvlup = other.xp_required_to_lvlup;
    this->xp_value = other.xp_value;
    this->level = other.level;

    this->is_champion = other.is_champion;
    this->is_hero = other.is_hero;
    this->timed_attr_effects = new std::vector<TimedEffect*>(other.timed_attr_effects->begin(), other.timed_attr_effects->end());
    this->timed_spell_effects = new std::vector<TimedEffect*>(other.timed_spell_effects->begin(), other.timed_spell_effects->end());

    this->my_tile = other.my_tile;

    this->representation = new Representation(*other.representation);
    this->inventory = new Inventory(*other.inventory);
    this->inventory->master = other.inventory->master;
    this->attrs = new AttributeContainer(*other.attrs);
    this->attrs->owner = this;
    this->spells = new spell_vec_t(other.spells->begin(), other.spells->end());

    this->actors_in_sight = new actor_vec_t(other.actors_in_sight->begin(), other.actors_in_sight->end());

    this->thinker = new Thinker(*other.thinker);
    this->thinker->master = this;

    this->is_active = other.is_active;

    this->is_moving_left = other.is_moving_left;
    this->is_moving_right = other.is_moving_right;
    this->is_moving_up = other.is_moving_up;
    this->is_moving_down = other.is_moving_down;
    this->is_sneaking = other.is_sneaking;
    this->is_defending = other.is_defending;

    this->l_path = new TCODPath(*other.l_path);
    this->has_attacked = other.has_attacked;


};

TCODImage* Actor::get_image()
{
    TCODImage* img = new TCODImage(this->img_path.c_str());
    img->setKeyColor(TCODColor(255, 0, 255)); //XNA Pink
    return img;
};

bool Actor::try_detect(Actor* target)
{
    int base_hidden_stat = 100;
    int hidden_bonus = 0;
    if (target->is_sneaking)
    {
        hidden_bonus += 800;
    };
    // int hidden_bonus = 0; //TODO use target bonus hidden
    int net_hidden_stat = base_hidden_stat + hidden_bonus;
    int result = Game::stat_rolls_rng->getInt(0, net_hidden_stat);

    int base_detection_stat = 95;
    int detection_stat = 0; //TODO use this detection stat
    int net_detection = base_detection_stat + detection_stat;
    return result < net_detection;
    // return false;
};

Actor::~Actor()
{
    delete this->timed_attr_effects;
    delete this->timed_spell_effects;

    delete this->actors_in_sight;
    //std::cout << "DELETE ACTOR: " << this->inventory << std::endl;
    delete this->inventory;
    delete this->representation;
    delete this->attrs;
    delete this->spells;
    delete this->thinker;
};

void Actor::resetIsMoving()
{
    this->is_moving_up = false; 
    this->is_moving_right = false;
    this->is_moving_down = false;
    this->is_moving_left = false;
};

std::string Actor::get_name()
{
    return name;
};

const char* Actor::get_name_c()
{
    return name.c_str();
};

bool Actor::is_actor_in_sight(Actor* actor)
{

    actor_vec_t::iterator it;
    it = std::find(this->actors_in_sight->begin(), this->actors_in_sight->end(),  actor);
    if (it != this->actors_in_sight->end()) 
    {
        //std::cout << "hes in sight" << std::endl;
        return true; 
    }
    else 
    {
        return false; 
    }
};

void Actor::mark_as_unseen(Actor * actor)
{
    // std::cout << "removing from sight" << std::endl;
    actor_vec_t* ais = this->actors_in_sight;
    //if the actor is in sight and isnt this actor:
    if (actor == this) 
    {
        return;
    }
    ais->erase(std::remove(ais->begin(), ais->end(), actor), ais->end());

}
void Actor::mark_as_seen(int x, int y, Actor * actor)
{
    actor_vec_t* ais = this->actors_in_sight;
    //if the actor is in sight and isnt this actor:
    if (actor == this) 
    {
        return;
    }
    if (std::find(ais->begin(), ais->end(), actor) != ais->end())
    {
    }
    else 
    {
        this->actors_in_sight->push_back(actor);
    }

}

void Actor::put_person( Tile * next_tile, int new_x, int new_y)
{	//puts a person on a tile, resets the old tile

    if (my_tile != NULL){
        my_tile->makeUnoccupied(this); 
    }

    if (next_tile != NULL){
        this->my_tile = next_tile;
        next_tile->makeOccupied(this); 
    }
    else if (next_tile == NULL)
    {
        std::cout << "next tile is null" << std::endl;
    }
    else
    {
        std::cout << "else, this shouldn't be a possiblity" << std::endl;
    }

    if (new_x != -1 && new_y != -1)
    {
        this->x = new_x;
        this->y = new_y;
    }

};

void Actor::pick_up_item(Item* item)
{

    //add to inventory
    this->inventory->add_item(item);

    // remove from Tiles
    this->my_tile->pick_up_item(item);

};

bool Actor::has_attributes()
{
    return this->attrs != NULL;
};

Item* Actor::create_corpse()
{
    Item* corpse = new Item;
    corpse->repr->repr = '%';
    corpse->repr->setFGColor(*this->representation->fg_color, true, false, true);

    // std::string colored_corpse = colfg(TCODColor::red, " corpse");
    // corpse->name =  this->cls_name + colored_corpse;
    corpse->name = this->cls_name + " corpse";
    corpse->slot_type = slots_t::NoSlot;
    corpse->attr_effect->set_all_vals_to(0);
    corpse->attr_effect->health_regen_interval=1.25;
    corpse->attr_effect->health_regen_rate=1;
    corpse->attr_effect->hunger_current_val=-50;
    corpse->usable = true;
    corpse->equippable = false;

    return corpse;
};

int* Actor::get_direction_heading()
{
    int dir_array[2];

    //handle x coord
    if (this->is_moving_left) { dir_array[0] = -1; }
    else if (this->is_moving_right) { dir_array[0] =  1; }
    else { dir_array[0] = 0; };

    //handle y coord
    if (this->is_moving_up) { dir_array[1] = -1; }
    else if (this->is_moving_down) { dir_array[1] =  1; }
    else { dir_array[1] = 0; };

    return dir_array;
};


void Actor::printout_score()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y_%Ih%Mm%Ss",timeinfo);
    std::string str(buffer);

    std::string filepath = (get_data_path()+"necropolis_"+str+".log" );
    std::ofstream necro(filepath.c_str(), std::ofstream::out);

    //game version
    necro << Game::get_version() << std::endl;
    necro  << std::endl;
    //xp == score
    necro << "XP: " << Game::player->xp << std::endl;
    necro << "LEVEL: " << Game::player->level << std::endl;
    necro << "CLASS: " << Game::player->actor_class->name << std::endl;

    necro << std::endl;
    necro << "FLOOR: " << Game::current_map->depth << std::endl;
    necro << "TURNS: " << Game::turn_count << std::endl;
    necro << std::endl;
    //stats 
    necro << "KILLS: " << Game::stats->monsters_killed << std::endl;
    necro << "SPELLS CAST: " << Game::stats->spells_cast << std::endl;
    necro << "ITEMS USED:" << Game::stats->items_used << std::endl;
    necro << "PHYS DMG DEALT:" << Game::stats->damage_dealt << std::endl;
    necro << "PHYS DMG TAKEN:" << Game::stats->damage_taken << std::endl;
    necro << std::endl;

    necro << "----- STATS -----"  << std::endl;
    necro << Game::player->attrs->PrettyPrint() << std::endl;
    necro  << std::endl;

    //equipment
    necro << std::endl;

    //inventory
    necro << "----- INVENTORY -----"  << std::endl;
    for (auto it = Game::player->inventory->items->begin(); it!=Game::player->inventory->items->end(); it++)
    {
        necro << (*it)->name << std::endl;
        necro << (*it)->description << std::endl;
        necro << (*it)->attr_effect->oneline_str_colorless().c_str() << std::endl << std::endl;
    }
    necro  << std::endl;

    necro.close();
};

void Actor::die()
{
    Item* dropped_item = NULL;
    dropped_item = DropHandler::handle_for_actor(this);

    if (dropped_item != NULL)
    {
        this->my_tile->place_item_down(dropped_item);
    }

    if (this->combat != NULL)
    {
        Combat* last_attacker = this->combat->get_last_attacker();
        if (last_attacker != NULL && last_attacker->master == Game::player) //TODO why was this commented out?
            //if (last_attacker->master == Game::player)
        {
            Game::stats->monsters_killed++;
        };
    };

    //bloody the ground
    Representation* repr = this->my_tile->get_representation();
    Representation* new_repr = new FloorRepresentation;
    new_repr->repr = repr->repr;
    new_repr->setFGColor((*repr->fg_color)+(TCODColor::darkerRed*0.3), true, true, true);
    this->my_tile->set_representation(new_repr);

    this->my_tile->set_description("There is some blood on the ground.");

    //remove master from ai update list
    this->is_active = false;
    this->put_person(NULL, NULL, NULL);

    //FIXME this is because people can still 'see' the dead dude. removing only
    //from players view, will need to remove from everyone elses at some point
    Game::player->mark_as_unseen(this);

    //destroy ai
    if (this->thinker != NULL)
    {
        delete this->thinker;
    };

    new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(TCODColor::lighterRed, "%s died!"), this->name.c_str());

    int multiplier = 1;
    if (this == (Actor*)Game::player)
    {
        TCODSystem::setFps(0);
        for (int fade=255*multiplier; fade >= 0; fade --) {
            if (fade % multiplier == 0)
            {
                TCODConsole::setFade(fade/multiplier,TCODColor::red);
                TCODConsole::flush();
                printf("YOU'RE DEAD GIVE UP\n");
            };
        }
        TCODConsole::setFade(255, TCODColor::red);

        std::cout << "Death log is being prepared..." << std::endl;
        this->printout_score();

        TCODSystem::setFps(Game::fps_limit);
        Game::start_game();
        // exit(1);

    };

};
