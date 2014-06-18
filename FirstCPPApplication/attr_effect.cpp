#include "stdafx.h"

#include "attr_effect.h"
#include "attribute_container.h"
#include "attribute.h"
#include <actors/actor.h>
#include "utils.h"
#include "libtcod.hpp"
#include "combat.h"

bool TimedEffect::is_expired(long double turn_count)
{
    return (this->effect->duration + this->turn_applied) <= turn_count;
};

AttrEffect::AttrEffect()
{
    this->health_current_val = 100;
    this->health_max_val = 0;
    this->health_regen_rate = 0;
    this->health_regen_interval = 0; 

    this->mana_current_val = 0;
    this->mana_max_val = 0;
    this->mana_regen_rate = 0;
    this->mana_regen_interval = 0; 

    this->armor_current_val = 0;
    this->armor_max_val = 0;
    this->armor_regen_rate = 0;
    this->armor_regen_interval = 0; 
    this->armor = new Armor();

    this->damage_current_val = 0;
    this->damage_max_val = 0;
    this->damage_regen_rate = 0;
    this->damage_regen_interval = 0; 
    this->damage = new Damage();

    this->hunger_current_val = 0;
    this->hunger_max_val = 0;
    this->hunger_regen_rate = 0;
    this->hunger_regen_interval = 0; 


    this->duration = -1;

    this->actors_applied_to = new std::vector<applied_to_s*>;
};

bool AttrEffect::is_timed()
{
    return this->duration != NO_DURATION;
};

void AttrEffect::set_all_vals_to(int new_val)
{
    this->set_health_vals_to(new_val);
    this->set_mana_vals_to(new_val);
    this->set_armor_vals_to(new_val);
    this->set_damage_vals_to(new_val);
    this->set_hunger_vals_to(new_val);
};

void AttrEffect::set_rng_health(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->health_current_val = val;
    this->health_max_val = val;
};

void AttrEffect::set_rng_mana(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->mana_current_val = val;
    this->mana_max_val = val;
};

int AttrEffect::set_rng_armor(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->armor_current_val = val;
    this->armor_max_val = val;
    return val;
};

int AttrEffect::set_rng_damage(TCODRandom* rng, int min, int max, int med)
{
    int val = rng->getInt(min, max, med);
    this->damage_current_val = val;
    this->damage_max_val = val;
    return val;
};

void AttrEffect::set_health_vals_to(int new_val)
{
    this->health_current_val = new_val;
    this->health_max_val = new_val;
    this->health_regen_rate = new_val;
    this->health_regen_interval = new_val; 
};

void AttrEffect::set_mana_vals_to(int new_val)
{
    this->mana_current_val = new_val;
    this->mana_max_val = new_val;
    this->mana_regen_rate = new_val;
    this->mana_regen_interval = new_val; 
};

void AttrEffect::set_armor_vals_to(int new_val)
{
    this->armor_current_val = new_val;
    this->armor_max_val = new_val;
    this->armor_regen_rate = new_val;
    this->armor_regen_interval = new_val; 
};

void AttrEffect::set_damage_vals_to(int new_val)
{
    this->damage_current_val = new_val;
    this->damage_max_val = new_val;
    this->damage_regen_rate = new_val;
    this->damage_regen_interval = new_val; 
};

void AttrEffect::set_hunger_vals_to(int new_val)
{
    this->hunger_current_val = new_val;
    this->hunger_max_val = new_val;
    this->hunger_regen_rate = new_val;
    this->hunger_regen_interval = new_val; 
};

bool AttrEffect::already_applied_health(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->health.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_mana(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return (*it)->mana.all;
        };

    };
    return false;
};

bool AttrEffect::already_applied_armor(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->armor.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_damage(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->damage.all);
        };

    };
    return false;
};

bool AttrEffect::already_applied_hunger(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->hunger.all);
        };

    };
    return false;
};

void AttrEffect::unmark_applied_all(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            (*it)->health.all = false;
            (*it)->mana.all = false;
            (*it)->armor.all  = false;
            (*it)->damage.all = false;
        };

    };
};
bool AttrEffect::already_applied_all(Actor* actor)
{

    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
            return ((*it)->health.all && (*it)->mana.all && (*it)->armor.all  && (*it)->damage.all );
        };

    };

    //if actor in list of people this item's already applied to, no reason to do
    // //it again
    // auto it = std::find(this->actors_applied_to->begin(), this->actors_applied_to->end(), actor);

    // return  it != this->actors_applied_to->end();

    // // The thing is that I need to sort out how to apply the item's effects to
    // // the actor once, because if it gets reapplied they might be invincible or
    // // gaining way too much health for ever.
    // //TODO TODO TODO

    return false;
};


void AttrEffect::mark_applied_health(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->health.all = true);
        };

    };
};


void AttrEffect::mark_applied_mana(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->mana.all = true);
        };

    };
};

void AttrEffect::mark_applied_armor(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->armor.all = true);
        };

    };
};

void AttrEffect::mark_applied_damage(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->damage.all = true);
        };

    };
};

void AttrEffect::mark_applied_hunger(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->hunger.all = true);
        };

    };
};

void AttrEffect::mark_applied_all(Actor* actor)
{
    //loop through the actors_applied_to vector and find one with the actor
    //passed in
    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
    for (it; it != this->actors_applied_to->end(); ++it)
    {
        if ((*it)->actor == actor)
        {
           ((*it)->health.all = true);
           ((*it)->mana.all = true);
           ((*it)->armor.all = true);
           ((*it)->damage.all = true);
        };

    };
};


//void ItemEffect::mark_applied_all(Actor* actor)
//{
//    //loop through the actors_applied_to vector and find one with the actor
//    //passed in
//    std::vector<applied_to_s*>::iterator it = this->actors_applied_to->begin();
//    for (it; it != this->actors_applied_to->end(); ++it)
//    {
//        if ((*it)->actor == actor)
//        {
//           ((*it)->all = true);
//        };
//
//    };

    // auto it = std::find(this->actors_applied_to->begin(), this->actors_applied_to->end(), actor);

    // if ( it != this->actors_applied_to->end() )
    // {
    //     //do nothing because it already exists in the vector
    // }
    // else
    // {
    //     this->actors_applied_to->push_back(actor);
    // };
//};

void AttrEffect::ApplyAllEffects(Actor* actor)
{
    this->ApplyHealthEffects(actor);
    this->ApplyManaEffects(actor);
    this->ApplyArmorEffects(actor);
    this->ApplyDamageEffects(actor);
    this->ApplyHungerEffects(actor);
    if (actor->combat!= NULL)
        actor->combat->TryToDie();
    else
        std::cout << "this thing has no combat... why are you a dick?" << std::endl;
};

void AttrEffect::ApplyHealthEffects(Actor* actor)
{
    if (! this->already_applied_health(actor))
    {
        actor->attrs->health->AddToMaxVal(this->health_max_val);
        actor->attrs->health->AddToCurrentVal(this->health_current_val);
        actor->attrs->health->AddToRegenRate(this->health_regen_rate);
        actor->attrs->health->AddToRegenInterval(this->health_regen_interval);
        this->mark_applied_health(actor);
    }
};

void AttrEffect::ApplyManaEffects(Actor* actor)
{
    if (! this->already_applied_mana(actor))
    {
        actor->attrs->mana->AddToMaxVal(this->mana_max_val);
        actor->attrs->mana->AddToCurrentVal(this->mana_current_val);
        actor->attrs->mana->AddToRegenRate(this->mana_regen_rate);
        actor->attrs->mana->AddToRegenInterval(this->mana_regen_interval);
        this->mark_applied_mana(actor);
    }
};

void AttrEffect::ApplyArmorEffects(Actor* actor)
{
    if (! this->already_applied_armor(actor))
    {
        actor->attrs->armor->AddToMaxVal(this->armor_max_val);
        actor->attrs->armor->AddToCurrentVal(this->armor_current_val);
        actor->attrs->armor->AddToRegenRate(this->armor_regen_rate);
        actor->attrs->armor->AddToRegenInterval(this->armor_regen_interval);
        this->mark_applied_armor(actor);
    }
};

void AttrEffect::ApplyDamageEffects(Actor* actor)
{
    if (! this->already_applied_damage(actor))
    {
        actor->attrs->damage->AddToMaxVal(this->damage_max_val);
        actor->attrs->damage->AddToCurrentVal(this->damage_current_val);
        actor->attrs->damage->AddToRegenRate(this->damage_regen_rate);
        actor->attrs->damage->AddToRegenInterval(this->damage_regen_interval);
        this->mark_applied_damage(actor);
    }
};

void AttrEffect::ApplyHungerEffects(Actor* actor)
{
    if (! this->already_applied_hunger(actor))
    {
        actor->attrs->hunger->AddToMaxVal(this->hunger_max_val);
        actor->attrs->hunger->AddToCurrentVal(this->hunger_current_val);
        actor->attrs->hunger->AddToRegenRate(this->hunger_regen_rate);
        actor->attrs->hunger->AddToRegenInterval(this->hunger_regen_interval);
        this->mark_applied_hunger(actor);
    }
};

void AttrEffect::RemoveAllEffects(Actor* actor)
{
    this->RemoveHealthEffects(actor);
    this->RemoveManaEffects(actor);
    this->RemoveArmorEffects(actor);
    this->RemoveDamageEffects(actor);
};

void AttrEffect::RemoveHealthEffects(Actor* actor)
{
    actor->attrs->health->RemoveFromCurrentVal(this->health_current_val);
    actor->attrs->health->RemoveFromMaxVal(this->health_max_val);
    actor->attrs->health->RemoveFromRegenRate(this->health_regen_rate);
    actor->attrs->health->RemoveFromRegenInterval(this->health_regen_interval);
};

void AttrEffect::RemoveManaEffects(Actor* actor)
{
    actor->attrs->mana->RemoveFromCurrentVal(this->mana_current_val);
    actor->attrs->mana->RemoveFromMaxVal(this->mana_max_val);
    actor->attrs->mana->RemoveFromRegenRate(this->mana_regen_rate);
    actor->attrs->mana->RemoveFromRegenInterval(this->mana_regen_interval);
};

void AttrEffect::RemoveArmorEffects(Actor* actor)
{
    actor->attrs->armor->RemoveFromCurrentVal(this->armor_current_val);
    actor->attrs->armor->RemoveFromMaxVal(this->armor_max_val);
    actor->attrs->armor->RemoveFromRegenRate(this->armor_regen_rate);
    actor->attrs->armor->RemoveFromRegenInterval(this->armor_regen_interval);
};

void AttrEffect::RemoveDamageEffects(Actor* actor)
{
    actor->attrs->damage->RemoveFromCurrentVal(this->damage_current_val);
    actor->attrs->damage->RemoveFromMaxVal(this->damage_max_val);
    actor->attrs->damage->RemoveFromRegenRate(this->damage_regen_rate);
    actor->attrs->damage->RemoveFromRegenInterval(this->damage_regen_interval);
};


std::string AttrEffect::full_str()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back("HCV: "+std::to_string((long double)this->health_current_val));
    string_vec.push_back("HMV: "+std::to_string((long double)this->health_max_val));
    string_vec.push_back("HRR: "+std::to_string((long double)this->health_regen_rate));
    string_vec.push_back("HRI: "+std::to_string((long double)this->health_regen_interval));

    string_vec.push_back("MCV: "+std::to_string((long double)this->mana_current_val));
    string_vec.push_back("MMV: "+std::to_string((long double)this->mana_max_val));
    string_vec.push_back("MRR: "+std::to_string((long double)this->mana_regen_rate));
    string_vec.push_back("MRI: "+std::to_string((long double)this->mana_regen_interval));

    string_vec.push_back("ACV: "+std::to_string((long double)this->armor_current_val));
    string_vec.push_back("AMV: "+std::to_string((long double)this->armor_max_val));
    string_vec.push_back("ARR: "+std::to_string((long double)this->armor_regen_rate));
    string_vec.push_back("ARI: "+std::to_string((long double)this->armor_regen_interval));

    string_vec.push_back("DCV: "+std::to_string((long double)this->damage_current_val));
    string_vec.push_back("DMV: "+std::to_string((long double)this->damage_max_val));
    string_vec.push_back("DRR: "+std::to_string((long double)this->damage_regen_rate));
    string_vec.push_back("DRI: "+std::to_string((long double)this->damage_regen_interval));

    if (string_vec.size() != 0)
        string_vec.push_back("%c");

    return StringJoin(string_vec, '\n', false);

};

std::string AttrEffect::small_convert(std::string prefix, int val)
{
    if (val != 0)
    {
        return prefix + std::to_string((long double)val);
    }
    else
    {
        return "";
    };

};

// std::vector<TCOD_colctrl_t> AttrEffect::oneline_str_colours_FIXED()
// {
// };


std::string buffer_color(std::string input, int val, int fore_or_back, TCODColor color)
{
    char buffer[999];
    if (val == 0)
    {
        return "";
    };

    int r = std::max((double)1, (double)color.r);
    int g = std::max((double)1, (double)color.g);
    int b = std::max((double)1, (double)color.b);
    sprintf(buffer, input.c_str(), fore_or_back, r, g, b, val, TCOD_COLCTRL_STOP);

    return std::string(buffer);
};

std::string AttrEffect::oneline_str_FIXED()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    TCODColor health_color = TCODColor::desaturatedGreen;
    TCODConsole::setColorControl(TCOD_COLCTRL_1, health_color, TCODColor::black);
    TCODColor mana_color = TCODColor::desaturatedBlue;
    TCODConsole::setColorControl(TCOD_COLCTRL_2, mana_color, TCODColor::black);
    TCODColor armor_color = TCODColor::lightGrey;
    TCODConsole::setColorControl(TCOD_COLCTRL_3, armor_color, TCODColor::black);
    TCODColor damage_color = TCODColor::desaturatedRed;
    TCODConsole::setColorControl(TCOD_COLCTRL_4, damage_color, TCODColor::black);

    std::stringstream ss;

    ss << buffer_color("%c%c%c%cHCV: %i%c ", (int)this->health_current_val, TCOD_COLCTRL_FORE_RGB, health_color);
    ss << buffer_color("%c%c%c%cHMV: %i%c ", (int)this->health_max_val, TCOD_COLCTRL_FORE_RGB, health_color);
    ss << buffer_color("%c%c%c%cHRR: %i%c ", (int)this->health_regen_rate, TCOD_COLCTRL_FORE_RGB, health_color);
    ss << buffer_color("%c%c%c%cHRI: %i%c ", (int)this->health_regen_interval, TCOD_COLCTRL_FORE_RGB, health_color);

    ss << buffer_color("%c%c%c%cMCV: %i%c ", (int)this->mana_current_val, TCOD_COLCTRL_FORE_RGB, mana_color);
    ss << buffer_color("%c%c%c%cMMV: %i%c ", (int)this->mana_max_val, TCOD_COLCTRL_FORE_RGB, mana_color);
    ss << buffer_color("%c%c%c%cMRR: %i%c ", (int)this->mana_regen_rate, TCOD_COLCTRL_FORE_RGB, mana_color);
    ss << buffer_color("%c%c%c%cMRI: %i%c ", (int)this->mana_regen_interval, TCOD_COLCTRL_FORE_RGB, mana_color);

    ss << buffer_color("%c%c%c%cACV: %i%c ", (int)this->armor_current_val, TCOD_COLCTRL_FORE_RGB, armor_color);
    ss << buffer_color("%c%c%c%cAMV: %i%c ", (int)this->armor_max_val, TCOD_COLCTRL_FORE_RGB, armor_color);
    ss << buffer_color("%c%c%c%cARR: %i%c ", (int)this->armor_regen_rate, TCOD_COLCTRL_FORE_RGB, armor_color);
    ss << buffer_color("%c%c%c%cARI: %i%c ", (int)this->armor_regen_interval, TCOD_COLCTRL_FORE_RGB, armor_color);

    ss << buffer_color("%c%c%c%cDCV: %i%c ", (int)this->damage_current_val, TCOD_COLCTRL_FORE_RGB, damage_color);
    ss << buffer_color("%c%c%c%cDMV: %i%c ", (int)this->damage_max_val, TCOD_COLCTRL_FORE_RGB, damage_color);
    ss << buffer_color("%c%c%c%cDRR: %i%c ", (int)this->damage_regen_rate, TCOD_COLCTRL_FORE_RGB, damage_color);
    ss << buffer_color("%c%c%c%cDRI: %i%c ", (int)this->damage_regen_interval, TCOD_COLCTRL_FORE_RGB, damage_color);

    ss << buffer_color("%c%c%c%cNRM: %i%c ", (int)this->damage->normal, TCOD_COLCTRL_FORE_RGB, damage_color);
    ss << buffer_color("%c%c%c%cFIR: %i%c ", (int)this->damage->fire, TCOD_COLCTRL_FORE_RGB, TCODColor::desaturatedRed);
    ss << buffer_color("%c%c%c%cWTR: %i%c ", (int)this->damage->water, TCOD_COLCTRL_FORE_RGB, TCODColor::desaturatedBlue);
    ss << buffer_color("%c%c%c%cLIF: %i%c ", (int)this->damage->life, TCOD_COLCTRL_FORE_RGB, TCODColor::lightGreen);
    ss << buffer_color("%c%c%c%cDTH: %i%c ", (int)this->damage->death, TCOD_COLCTRL_FORE_RGB, TCODColor::darkestGrey);
    ss << buffer_color("%c%c%c%cCTL: %i%c ", (int)this->damage->crystal, TCOD_COLCTRL_FORE_RGB, TCODColor::darkPurple);
    ss << buffer_color("%c%c%c%cSPR: %i%c ", (int)this->damage->spectre, TCOD_COLCTRL_FORE_RGB, TCODColor::magenta);

    return ss.str();
    return StringJoin(string_vec, ' ', true);

};

std::vector<TCOD_colctrl_t> AttrEffect::oneline_str_colours()
{
    TCODColor health_color = TCODColor::desaturatedGreen;
    TCODConsole::setColorControl(TCOD_COLCTRL_1, health_color, TCODColor::black);
    TCODColor mana_color = TCODColor::desaturatedBlue;
    TCODConsole::setColorControl(TCOD_COLCTRL_2, mana_color, TCODColor::black);
    TCODColor armor_color = TCODColor::lightGrey;
    TCODConsole::setColorControl(TCOD_COLCTRL_3, armor_color, TCODColor::black);
    TCODColor damage_color = TCODColor::desaturatedRed;
    TCODConsole::setColorControl(TCOD_COLCTRL_4, damage_color, TCODColor::black);
    std::vector<TCOD_colctrl_t>  color_vector;   
    //for each attr in this item effect that isn't 0, return a attr specific
    //color
    if (this->health_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    if (this->health_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_1);
    
    if (this->mana_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_2);
    if (this->mana_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_2);

    if (this->armor_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_3);
    if (this->armor_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_3);

    if (this->damage_current_val != 0)
        color_vector.push_back(TCOD_COLCTRL_4);
    if (this->damage_max_val != 0)
        color_vector.push_back(TCOD_COLCTRL_4);
    if (this->damage_regen_rate != 0)
        color_vector.push_back(TCOD_COLCTRL_4);
    if (this->damage_regen_interval != 0)
        color_vector.push_back(TCOD_COLCTRL_4);

    //color_vector.push_back(TCOD_COLCTRL_STOP);
    return color_vector;
    
};

std::string AttrEffect::oneline_str()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back(this->small_convert("%cHCV: ", (int)this->health_current_val));
    string_vec.push_back(this->small_convert("%cHMV: ", (int)this->health_max_val));
    string_vec.push_back(this->small_convert("%cHRR: ", (int)this->health_regen_rate));
    string_vec.push_back(this->small_convert("%cHRI: ", (int)this->health_regen_interval));

    string_vec.push_back(this->small_convert("%cMCV: ", (int)this->mana_current_val));
    string_vec.push_back(this->small_convert("%cMMV: ", (int)this->mana_max_val));
    string_vec.push_back(this->small_convert("%cMRR: ", (int)this->mana_regen_rate));
    string_vec.push_back(this->small_convert("%cMRI: ", (int)this->mana_regen_interval));

    string_vec.push_back(this->small_convert("%cACV: ", (int)this->armor_current_val));
    string_vec.push_back(this->small_convert("%cAMV: ", (int)this->armor_max_val));
    string_vec.push_back(this->small_convert("%cARR: ", (int)this->armor_regen_rate));
    string_vec.push_back(this->small_convert("%cARI: ", (int)this->armor_regen_interval));

    string_vec.push_back(this->small_convert("%cDCV: ", (int)this->damage_current_val));
    string_vec.push_back(this->small_convert("%cDMV: ", (int)this->damage_max_val));
    string_vec.push_back(this->small_convert("%cDRR: ", (int)this->damage_regen_rate));
    string_vec.push_back(this->small_convert("%cDRI: ", (int)this->damage_regen_interval));

    return StringJoin(string_vec, ' ', true);

};

std::string AttrEffect::oneline_str_colorless()
{

    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back(this->small_convert("HCV: ", (int)this->health_current_val));
    string_vec.push_back(this->small_convert("HMV: ", (int)this->health_max_val));
    string_vec.push_back(this->small_convert("HRR: ", (int)this->health_regen_rate));
    string_vec.push_back(this->small_convert("HRI: ", (int)this->health_regen_interval));

    string_vec.push_back(this->small_convert("MCV: ", (int)this->mana_current_val));
    string_vec.push_back(this->small_convert("MMV: ", (int)this->mana_max_val));
    string_vec.push_back(this->small_convert("MRR: ", (int)this->mana_regen_rate));
    string_vec.push_back(this->small_convert("MRI: ", (int)this->mana_regen_interval));

    string_vec.push_back(this->small_convert("ACV: ", (int)this->armor_current_val));
    string_vec.push_back(this->small_convert("AMV: ", (int)this->armor_max_val));
    string_vec.push_back(this->small_convert("ARR: ", (int)this->armor_regen_rate));
    string_vec.push_back(this->small_convert("ARI: ", (int)this->armor_regen_interval));

    string_vec.push_back(this->small_convert("DCV: ", (int)this->damage_current_val));
    string_vec.push_back(this->small_convert("DMV: ", (int)this->damage_max_val));
    string_vec.push_back(this->small_convert("DRR: ", (int)this->damage_regen_rate));
    string_vec.push_back(this->small_convert("DRI: ", (int)this->damage_regen_interval));

    return StringJoin(string_vec, ' ', true);

};

