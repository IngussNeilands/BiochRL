#include "stdafx.h"
#include "statistics.h"

Statistics::Statistics() 
{
    this->reset_stats();
};

void Statistics::reset_stats()
{
    this->monsters_killed = 0;
    this->spells_cast = 0;
    this->items_used = 0;
    this->damage_dealt = 0;
    this->damage_taken = 0;
};
