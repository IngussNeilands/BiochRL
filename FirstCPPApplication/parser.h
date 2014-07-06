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
        

};

#endif
