#pragma once
#ifndef SLUDGE_H
#define SLUDGE_H
#include <actors\Person.h>

class Sludge : public Person
{
    public:
        Sludge(std::string name, int age, int x, int y, char repr);
        static const int Sludge::pack_size = 10;
        void championize();
        void die() ;
};
#endif
