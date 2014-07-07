#pragma once
#ifndef PARSER_H
#define PARSER_H

#include "class.h"

class Parser
{
    public:
        TCODParser* tcod_parser;
        Parser();
        ~Parser();
        TCODParser* init_parser();
        bool get_enable_music();
        float get_music_volume();
        ClassType get_preferred_class_type();

        int get_spawning_rng_seed();
        int get_item_spawn_rng_seed();
        int get_linear_rng_seed();
        int get_event_rng_seed();
        int get_stat_rolls_rng_seed();
        int get_dungeon_builder_rng_seed();
        

};

#endif
