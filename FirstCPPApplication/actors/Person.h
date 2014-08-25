#pragma once
#ifndef PERSON_H
#define PERSON_H
#include <sstream>
#include <string>

#include "Actor.h"


class Combat;
class Tile;
class Actor;
class Game;

class Person : public Actor
{
    public:
        // std::string name;
        int age;

        Person();
        Person(std::string name, int age, int x, int y, char repr);
        ~Person();

        void attack(Actor * target);
        void update();
        void championize();
        bool talk_to(Actor* occupant);
        std::string Person::talk_wrap(Actor* target, std::string text);
};
#endif
