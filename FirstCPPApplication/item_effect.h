#pragma once
#ifndef ITEM_EFFECT_H
#define ITEM_EFFECT_H

#include <vector>
#include <iostream>

const int NO_DURATION = -1;

class Actor;
class Person;
class Combat;
class Person;
class Item;
class Inventory;
class Tile;
class Item;
class Attribute;

#include "libtcod.hpp"


// class TCODColor;
// extern class TCOD_colctrl_t;

struct type_applied_s 
{
    bool current_val;
    bool max_val;
    bool regen_rate;
    bool interval;
    bool all;
};

struct applied_to_s 
{
    type_applied_s health;
    type_applied_s mana;
    type_applied_s armor;
    type_applied_s damage;
    type_applied_s hunger;
    Actor* actor;
};

class ItemEffect;

class TimedEffect
{
    public:
        ItemEffect* effect;
        long double turn_applied;

        bool is_expired(long double turn_count);
};

class ItemEffect
{
    public:
        int duration;
        bool is_timed();

        int health_current_val;
        int health_max_val;
        int health_regen_rate;
        int health_regen_interval; 

        int mana_current_val;
        int mana_max_val;
        int mana_regen_rate;
        int mana_regen_interval; 

        int armor_current_val;
        int armor_max_val;
        int armor_regen_rate;
        int armor_regen_interval; 

        int damage_current_val;
        int damage_max_val;
        int damage_regen_rate;
        int damage_regen_interval; 

        int hunger_current_val;
        int hunger_max_val;
        int hunger_regen_rate;
        int hunger_regen_interval; 


        applied_to_s  applied_to;
        std::vector<applied_to_s*>* actors_applied_to;

        ItemEffect();
        void ApplyAllEffects(Actor* actor);
        void ApplyHealthEffects(Actor* actor);
        void ApplyManaEffects(Actor* actor);
        void ApplyArmorEffects(Actor* actor);
        void ApplyDamageEffects(Actor* actor);
        void ApplyHungerEffects(Actor* actor);

        void RemoveAllEffects(Actor* actor);
        void RemoveHealthEffects(Actor* actor);
        void RemoveManaEffects(Actor* actor);
        void RemoveArmorEffects(Actor* actor);
        void RemoveDamageEffects(Actor* actor);
        void RemoveHungerEffects(Actor* actor);

        bool already_applied_all(Actor* actor);
        bool already_applied_health(Actor* actor);
        bool already_applied_mana(Actor* actor);
        bool already_applied_armor(Actor* actor);
        bool already_applied_damage(Actor* actor);
        bool already_applied_hunger(Actor* actor);

        void mark_applied_all(Actor* actor);
        void mark_applied_health(Actor* actor);
        void mark_applied_mana(Actor* actor);
        void mark_applied_armor(Actor* actor);
        void mark_applied_damage(Actor* actor);
        void mark_applied_hunger(Actor* actor);

        void set_rng_health(TCODRandom* rng, int min, int max, int med);
        void set_rng_mana(TCODRandom* rng, int min, int max, int med);
        void set_rng_armor(TCODRandom* rng, int min, int max, int med);
        void set_rng_damage(TCODRandom* rng, int min, int max, int med);
        void set_rng_hunger(TCODRandom* rng, int min, int max, int med);

        void unmark_applied_all(Actor* actor);

        std::string ItemEffect::small_convert(std::string prefix, int val);
        std::string full_str();
        std::string oneline_str();
        std::string oneline_str_colorless();
        // std::vector<TCODColor> oneline_str_colours();
        std::vector<TCOD_colctrl_t> oneline_str_colours();

        void set_all_vals_to(int new_val);
        void set_health_vals_to(int new_val);
        void set_mana_vals_to(int new_val);
        void set_armor_vals_to(int new_val);
        void set_damage_vals_to(int new_val);
        void set_hunger_vals_to(int new_val);
};

#endif
