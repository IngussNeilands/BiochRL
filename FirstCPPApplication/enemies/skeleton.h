#ifndef SKELETON_H
#define SKELETON_H

#include <actors\Person.h>

class Skeleton : public Person
{
    public:
        Skeleton(std::string name, int age, int x, int y, char repr);
        static const int pack_size = 7;
        static const int preferred_pack_size = 3;
};

#endif
