#pragma once
#ifndef COMBAT_H
#define COMBAT_H
#include <sstream>
#include <vector>
// #include <actors/actor.h>
// #include <actors\Person.h>

class Person; 
class Actor; 
class Game;
class Damage;

class Combat 
{
    private:
        std::string default_name;
    public:
        // std::string name;
        Actor* master;

        bool is_dead;
        bool was_attacked;
        Actor* last_victim;
        std::vector<Combat*> * attackers;

        Combat (std::string name, int max_hp, Person* master, char representation = 'p' );

        void Attack(Combat* Combat_target, Damage* dmg);
        void TakeDamage(Combat* Combat_attacker, Damage* dmg);
        Combat* GetLastAttacker();
        int adjust_damage_to_armor(Damage* dmg);
        void RememberAttacker(Combat* Combat_attacker, bool mark_the_attk);

        void TryToDie();
        bool CheckDeath();
        void Die();

        void update();
        void assign_to_master(Person* master);

        void GiveExp(int exp_to_gain);

        void LevelUp(int levels = 1);
        void LevelUpStats(int levels);
        void LevelUpSkills(int levels);

        void printout();
};

class Damage 
{
    public:
        int normal;
        int fire;
        int water;
        int death;
        int life;
        int crystal;
        int spectre;

        Damage();

        int get_raw_total();

};

class Armor
{
    public:
        int normal;
        int fire;
        int water;
        int death;
        int life;
        int crystal;
        int spectre;

        Armor();
        int get_raw_total();

};
#endif
