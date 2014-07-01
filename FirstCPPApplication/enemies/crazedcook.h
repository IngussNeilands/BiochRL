#pragma once
#ifndef CRAZEDCOOK_H
#define CRAZEDCOOK_H

#include <actors\Person.h>
class CrazedCook : public Person
{
    public:
        CrazedCook(std::string name, int age, int x, int y, char repr);
        static const int CrazedCook::pack_size = 10;
        void championize();
        void die() ;
};
#endif
