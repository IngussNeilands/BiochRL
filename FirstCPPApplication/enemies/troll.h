#ifndef TROLL_H
#define TROLL_H

#include <actors\Person.h>

class Troll : public Person
{
    public:
        Troll(std::string name, int age, int x, int y, char repr);
        static const int Troll::pack_size = 5;
};

#endif
