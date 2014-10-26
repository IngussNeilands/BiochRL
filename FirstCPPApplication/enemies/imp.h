#pragma once
#ifndef IMP_H
#define IMP_H

#include <actors\Person.h>
class Imp : public Person
{
    public:
        Imp(std::string name, int age, int x, int y, char repr);
        static const int Imp::pack_size = 10;
};
#endif
