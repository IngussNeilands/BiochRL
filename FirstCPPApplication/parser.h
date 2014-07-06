#pragma once
#ifndef PARSER_H
#define PARSER_H

class Parser
{
    public:
        TCODParser* tcod_parser;
        Parser();
        ~Parser();
        TCODParser* init_parser();
        bool get_enable_music();
        float get_music_volume();
};

#endif
