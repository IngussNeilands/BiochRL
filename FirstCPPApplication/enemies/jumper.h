#pragma once
#ifndef JUMPER_H
#define JUMPER_H

#include <actors\Person.h>
class Jumper : public Person
{
    public:
        Jumper(std::string name, int age, int x, int y, char repr);
        static const int Jumper::pack_size = 10;
        void championize();
        void die() ;
};
#endif
