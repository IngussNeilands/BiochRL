#pragma once
#ifndef LIZARD_H
#define LIZARD_H

#include <actors\Person.h>
class SpinyLizard : public Person
{
    public:
        SpinyLizard(std::string name, int age, int x, int y, char repr);
        static const int SpinyLizard::pack_size = 10;
};
#endif
