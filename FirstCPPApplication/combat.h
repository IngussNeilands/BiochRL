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

class Combat 
{
    private:
        std::string default_name;
    public:
        // std::string name;
        Actor* master;
        char representation;

        bool is_dead;
        bool was_attacked;
        std::vector<Combat*> * attackers;

        Combat (std::string name, int max_hp, Person* master, char representation = 'p' );

        void Combat::Attack(Combat* Combat_target, int dmg);
        void Combat::TakeDamage(Combat* Combat_attacker, int dmg);
        bool Combat::CheckDeath();
        void Combat::RememberAttacker(Combat* Combat_attacker, bool mark_the_attk);
        void Combat::Die();
        Combat* Combat::GetLastAttacker();
        void Combat::update(Game* game);
        void Combat::assign_to_master(Person* master);

        void printout();
};
#endif
