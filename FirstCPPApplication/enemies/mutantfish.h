#pragma once
#ifndef MUTANTFISH_H
#define MUTANTFISH_H
#include <actors\Person.h>

class MutantFish : public Person
{
    public:
        MutantFish(std::string name, int age, int x, int y, char repr);
        static const int MutantFish::pack_size = 10;
};
#endif
