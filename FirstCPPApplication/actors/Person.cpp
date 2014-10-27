#include "stdafx.h"

#include <iostream>
#include <sstream>

#include "Person.h"

// #include "libtcod.hpp"

#include "thinker.h"
#include "inventory.h"
#include "equipment.h"
#include "Representation.h"
#include "tile.h"	
#include "attribute.h"
#include "attribute_container.h"
#include "combat.h"
#include "messages.h"
#include "game.h"
#include "ui.h"
#include "civilian.h"
#include "enums\hunger_threshold.h"
#include <utils.h>
#include <item.h>
#include <randsys.h>

Person::Person(std::string name, int age, int x, int y, char repr)
{
    this->cls_name = "Person";
    this->name = name;
    this->age = age;
    this->x = x;
    this->y = y;

    this->img_path = get_data_path()+"img/townsmen8x8.png";

    this->xp = 0;
    this->xp_this_level = 0;
    this->xp_required_to_lvlup = 200;
    this->xp_value = 10;
    this->level = 1;

    this->is_champion = false;
    this->pack_size = 0;


    //set its representation, aka color and char
    this->representation = new Representation;
    this->representation->repr = repr;
    this->representation->setFGColor(TCODColor::lighterFlame, true, true, true);

    Inventory* inventory =   new Inventory();
    this->inventory = inventory;
    this->inventory->master = this;
    this->equipment = new Equipment;
    this->equipment->master = this;

    is_fighter = true;

    combat = new Combat("temp name", 50, this, 't' );
    combat->assign_to_master(this);
    has_live_combat = true;

    my_tile = NULL;

    is_active = true;

    this->attrs->damage->max_val = 1;
    this->attrs->damage->current_val = 1;

};

Person::Person() : Actor()
{
    this->cls_name = "Person";
    this->name = "Unset Person name";
    this->age = 80085;
    this->x = 10;
    this->y = 10;

    this->dest_x = -1;
    this->dest_y = -1;


    //set its representation, aka color and char
    this->representation->repr = '~';

    Inventory* inventory =   new Inventory();
    this->inventory = inventory;
    this->inventory->master = this;
    this->equipment = new Equipment;
    this->equipment->master = this;

    is_fighter = true;

    combat = new Combat("temp2 name", 25, this, 't');
    combat->assign_to_master(this);
    has_live_combat = true;

    my_tile = NULL;
};

Person::~Person(){
    //TODO: safely remove Combat
    this->my_tile->makeUnoccupied(this);
};

void Person::update()
{
    //apply item basic item attributes of only equipped items if they've not
    //been equipped already
    this->equipment->Update();

    //update hunger
    Attribute* hunger = this->attrs->hunger;
    if (hunger->get_val_percentage() > WastingAwayHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.25;
    }
    else if (hunger->get_val_percentage() > NearDeathHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.1;
    }
    else if (hunger->get_val_percentage() > FamishedHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.05;
    }
    else if (hunger->get_val_percentage() > StarvingHunger)
    {
        this->attrs->health->regen_rate = -(float)this->attrs->health->max_val*0.025;
    }
    // else if (hunger->get_val_percentage() > VeryHunger)
    // {
    //     this->attrs->health->regen_rate -= (float)this->attrs->health->max_val*0.25;
    // }
    // else if (hunger->get_val_percentage() > DefaultHunger)
    // {
    //     this->attrs->health->regen_rate -= (float)this->attrs->health->max_val*0.05;
    // }

    //item buffs
    std::vector<TimedEffect*>* timed_items = this->timed_attr_effects;
    auto it = timed_items->begin();
    for (it; it != timed_items->end(); )
    {
        if ((*it)->is_expired(Game::turn_count))
        {
            (*it)->effect->RemoveAllEffects(this);
            delete *(it);
            it = timed_items->erase(it);
        }
        else
        {
            it++;
        }
    }

    //spell bufs
    std::vector<TimedEffect*>* timed_spells = this->timed_spell_effects;
    it = timed_spells->begin();
    for (it; it != timed_spells->end();)
    {
        if ((*it)->is_expired(Game::turn_count))
        {
            (*it)->effect->RemoveAllEffects(this);
            delete *(it);
            it = timed_spells->erase(it);
        }
        else
        {
            it++;
        };
    }

    this->attrs->Update();

    //TODO intervals

    if (this->thinker != NULL)
    {
        thinker->update();
    }

    if (this->combat != NULL)
    {
        this->combat->try_to_die();
    };
};

void Person::attack(Actor * target)
{
    //only Persons and children have combats
    Damage* dmg = this->attrs->get_effective_damage();
    if (this->is_sneaking && target->thinker!=NULL && target->thinker->get_is_aware() == false)
    {
        dmg->normal = dmg->normal*1.5;
    };

    RandomWeightMap<bool> crit_rwm = RandomWeightMap<bool>();
    crit_rwm.add_item(true, 5);
    crit_rwm.add_item(false, 95);
    bool is_crit = crit_rwm.get_item(Game::stat_rolls_rng);
    if (is_crit)
    {
        dmg->normal *= 2;
        new Message(Ui::msg_handler_main, DAMAGE_GIVEN_MSG, colfg(TCODColor::lightAmber, this->name+" swings hard."));
    };

    combat->attack(((Person*)target)->combat, dmg); 
    delete dmg;
};

void Person::championize()
{
    this->is_champion = true;

    RandomWeightMap<std::string> prefix_rwm = RandomWeightMap<std::string>();
    prefix_rwm.add_item("Champion", 10);
    prefix_rwm.add_item("Heroic", 10);
    prefix_rwm.add_item("Legendary", 10);

    std::string prefix = prefix_rwm.get_item(Game::spawning_rng);
    TCODColor opposite = TCODColor::white - (*this->representation->fg_color);
    TCODColor new_color = TCODColor::lerp(opposite, *this->representation->fg_color, 0.1f);

    this->name = prefix+" "+this->name;
    this->representation->setFGColor(new_color, true, false, true);
    this->attrs->health->current_val+=this->attrs->health->current_val;
    this->attrs->health->max_val+=this->attrs->health->max_val;
    this->xp_value= (int)floor(this->xp_value*1.5);

}

std::string Person::talk_wrap(Actor* target, std::string text)
{
    return "He says, \""+text+"\"";
};

//incr turn
bool Person::talk_to(Actor* target)
{
    if (target->thinker->civilian != NULL)
    {
        std::string text = this->talk_wrap(target, target->thinker->civilian->talk_general_topic());
        new Message(Ui::msg_handler_main, CHAT_MSG, colfg(TCODColor::lighterAmber, text));

        if (target->thinker->civilian->is_shopkeep)
        {
            unsigned long long total_gold = target->thinker->civilian->sell_from_floor(Game::player);

            if (total_gold != 0)
            {
                Game::player->total_gold += total_gold;
                std::stringstream ss;
                ss << "You just earned: " << total_gold;
                ss << ", you now have: " << Game::player->total_gold << " gold.";
                std::string text = ss.str();
                new Message(Ui::msg_handler_main, CHAT_MSG, colfg(TCODColor::lighterAmber, text));
            }
        }
        else if (target->thinker->civilian->is_weaponsmith)
        {
            int upgrade_cost = 200;

            std::stringstream ss;
            if (Game::player->total_gold > upgrade_cost)
            {
                int upgrade_gain = target->thinker->civilian->upgrade_primary_weapon(Game::player);
                Game::player->total_gold -= upgrade_cost;
                // std::cout << "you just spent: " << upgrade_cost << std::endl;
                if (upgrade_gain != 0)
                {
                    ss << "You just gained: " << upgrade_gain;
                    ss << ", weapon does: " << Game::player->equipment->main_weapon->equipped_item->attr_effect->damage->get_raw_total() << " damage.";
                }
                else
                {
                    ss << "You must have a primary weapon equipped";

                };
                // std::cout << "you now have: " << Game::player->total_gold << std::endl;
            }
            else
            {
                ss << "The cost of upgrading your primary weapon is " << upgrade_cost << " gold.";
            };
            std::string text = ss.str();
            new Message(Ui::msg_handler_main, CHAT_MSG, colfg(TCODColor::lighterAmber, text));
        }
        else if (target->thinker->civilian->is_healer)
        {
            int upgrade_cost = 50;

            std::stringstream ss;
            if (Game::player->total_gold > upgrade_cost)
            {
                target->thinker->civilian->full_heal(Game::player);
                Game::player->total_gold -= upgrade_cost;
                ss << "The empath touches you and you feel rejuvinated! The empath looks sick.";
            }
            else
            {
                ss << "The cost of exchanging with the empath is " << upgrade_cost << " gold.";
            };
            std::string text = ss.str();
            new Message(Ui::msg_handler_main, CHAT_MSG, colfg(TCODColor::lighterAmber, text));
        };

        return true;
    }

    return false;
};
