#include "stdafx.h"
#include <sstream>
#include <typeinfo>
#include <cstdlib>

#include <libtcod.hpp>

#include "combat.h"
#include "attribute_container.h"
#include <actors\Person.h>
#include <actors\actor.h>
#include "attribute.h"
#include "tile.h"
#include <Representation.h>
#include "ui.h"
#include "messages.h"
#include "spells.h"
#include "attr_effect.h"
#include "class.h"
#include "thinker.h"


void Combat::printout(){

    std::cout << "\nCombat PRINTOUT" << std::endl;
    //cout << "NAME: " << name << endl;
    std::cout << "MAX HP: " << this->master->attrs->health->max_val << std::endl;

};


void Combat::assign_to_master(Person* master)
{
    this->master = master;
    // this->max_hp = (this->master->attrs->health->max_val);
    // this->cur_hp = (this->master->attrs->health->current_val);

    //if (name == default_name)
    //{
    //    // cout << "named default name: " << name << endl;
    //    // cout << "renaming to masters name" << endl;
    //    name = master->name + "'s Combat";
    //    // cout << "new name: " << name << endl;
    //}
    //else
    //{
    //    cout << "not named default, it's named instead: " << name << endl;
    //};
};

Combat::Combat(std::string name, int max_hp, Person* master, char representation)
{
    //name = name;

    // this->cur_hp = max_hp;
    // this->max_hp = max_hp;

    representation = representation;

    this->master = NULL;
    this->is_dead = false;

    this->attackers = new std::vector<Combat*>;
    this->was_attacked = false;

    this->last_victim = NULL;

};

void Combat::update()
{
    if (was_attacked == true)
    {
        //cout << "I (a Combat), " << name << ", was attacked, ouch!" << endl;

        // //retaliate
        // Combat* assailant = GetLastAttacker();
        // Attack(assailant, 1);


        // //calm the Combat down
        // was_attacked = false;
    };
};

void Combat::LevelUp(int levels)
{
    //reset, accounting for left over exp
    int i = 0;
    while (i < levels)
    {


        this->master->level+=1;
        std::cout << "NEW LEVEL IS: " << this->master->level << std::endl;
	//log(lvl * 10) * 100
        this->master->xp_required = std::floor(std::log(this->master->level *10.0f) * 100.0f);
        this->master->xp_this_level = this->master->xp_this_level - this->master->xp_required; 

        this->LevelUpStats(1);
        this->LevelUpSkills(1);
        i++;
    }

    //animate
    // Tile* tile = this->master->my_tile;
    // std::vector<Tile*>* adj_tiles = tile->getAdjacentTiles(2);
    // for (std::vector<Tile*>::iterator it = adj_tiles->begin(); it != adj_tiles->end(); ++it)
    // {
    //     (*it)->tile->representation->temp_bg_color = (TCODColor*)(&TCODColor::gold);
    // };
    // std::cout << "just exploded with color for a frame" << std::endl;
};

void Combat::LevelUpSkills(int levels)
{
    this->master->actor_class->LevelUpSkills(levels);
};

void Combat::LevelUpStats(int levels)
{
    this->master->actor_class->LevelUpStats(levels);
};

void Combat::GiveExp(int exp_to_gain)
{
    this->master->xp += exp_to_gain;
    this->master->xp_this_level += exp_to_gain;
    // calc if level up
    if (this->master->xp_this_level >= this->master->xp_required)
    {
        this->LevelUp();
    };
};

void Combat::Attack(Combat* combat_target, int dmg){
    //cout << "About to attack " << (*combat_target).name << endl;

    if (combat_target == NULL)
    {
        printf("target had no combat\n");
	return;
    }
    new Message(Ui::msg_handler_main, message_types_t::DAMAGE_GIVEN_MSG, "About to attack %s for %d damage.", combat_target->master->name.c_str(), dmg);
    combat_target->TakeDamage(this, dmg);
    this->last_victim = combat_target->master;

    bool is_target_dead = combat_target->CheckDeath();
    if (is_target_dead){
        //get opponents exp value
        int exp_to_gain = combat_target->master->xp_value;
        //add it to the master's exp
        this->GiveExp(exp_to_gain);
    };
};

void Combat::RememberAttacker(Combat* combat_attacker, bool mark_the_attk=true)
{
    if (mark_the_attk == true) 
    {
        was_attacked = true;
         if (this->master != Game::player)
         {
             this->master->thinker->target = combat_attacker->master;
         }
    };

    if(std::find(attackers->begin(), attackers->end(), combat_attacker) != attackers->end()) 
    {
        // printf("I've already been attacked by you.\n");
    }
    else 
    {
        attackers->push_back(combat_attacker);
        // printf("Oh hi, you're new.\n");
    }

};

void Combat::Die()
{
    printf("I've died!\n");
    //make position unblocked
    if (master != NULL)
    {
        master->Die();
    }
    else if (master == NULL)
    {
        printf("I've no master so he's not going to die, is he?\n");
    };

};

Combat* Combat::GetLastAttacker()
{
    // std::cout << "*** Retaliation ***" << std::endl;
    Combat * assailant;
    if (attackers->empty()) { return NULL; }
    assailant = attackers->back();
    // cout << "attacker: " << assailant->name << endl;

    return assailant;
};

void Combat::TryToDie()
{
    if (this->CheckDeath())
    {
        this->Die();
    };
}

void Combat::TakeDamage(Combat* combat_attacker, int dmg)
{
    if (dmg > 0) 
    {
        int adjusted_dmg = dmg-this->master->attrs->armor->current_val;
        (this->master->attrs->health->current_val)-= std::max(adjusted_dmg, 1);

        std::cout << this->master->name;
        std::cout << " took " << dmg << " damage! ";
        std::cout << "with " << this->master->attrs->health->current_val << "hp left.";
        std::cout << std::endl;

        //save attacker in history
        this->RememberAttacker(combat_attacker);

        this->TryToDie();
    }
    else
    {
        (this->master->attrs->health->current_val)-= dmg;

        std::cout << this->master->name;
        std::cout << " gained " << (-dmg) << " health! ";
        std::cout << "with " << this->master->attrs->health->current_val << "hp left.";
        std::cout << std::endl;
    };
};

bool Combat::CheckDeath()
{
    if (this->master->attrs->health->current_val <= 0  && !is_dead)
    {
        is_dead = true;
    }
    return is_dead;
};
