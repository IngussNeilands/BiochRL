#pragma once
#ifndef HULKINGMANTIS_H
#define HULKINGMANTIS_H

#include <actors\Person.h>
class HulkingMantis : public Person
{
    public:
        HulkingMantis(std::string name, int age, int x, int y, char repr);
        static const int HulkingMantis::pack_size = 10;
        void championize();
        void die() ;
};
#endif
