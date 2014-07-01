#pragma once
#ifndef IDOL_H
#define IDOL_H

#include <actors\Person.h>
class Idol : public Person
{
    public:
        Idol(std::string name, int age, int x, int y, char repr);
        static const int Idol::pack_size = 10;
        void championize();
        void die() ;
};

#endif
