#ifndef JACKAL_H
#define JACKAL_H

#include <actors\Person.h>

class Jackal : public Person
{
    public:
        Jackal(std::string name, int age, int x, int y, char repr);
        static const int pack_size = 15;
        static const int preferred_pack_size = 4;
};

#endif
