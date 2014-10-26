#pragma once
#ifndef WILDLING_H
#define WILDLING_H
#include <actors\Person.h>


#include <actors\Person.h>
class Wildling : public Person
{
    public:
        Wildling(std::string name, int age, int x, int y, char repr);
        static const int Wildling::pack_size = 10;
};
#endif
