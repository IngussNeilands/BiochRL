#pragma once
#ifndef STATISTICS_H
#define STATISTICS_H

/* class for tracking metagame stats, like monsters killed etc */
class Statistics
{
    public:
        unsigned int monsters_killed;
        unsigned int spells_cast;
        unsigned int items_used;
        unsigned int damage_dealt;
        unsigned int damage_taken;

        Statistics();
        void reset_stats();
};
#endif
