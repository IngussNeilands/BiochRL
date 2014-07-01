#pragma once
#ifndef HELPBOX_H
#define HELPBOX_H

class Tile;

class HelpBox 
{
    public:
        std::vector<std::string> messages;
        int height, width;
        int extra_padding;
        int left_pad, right_pad, top_pad, bot_pad;
        Tile* selected_tile;
        TCODConsole* con;
        TCODConsole* target_con;

        HelpBox();
        ~HelpBox();
        void initialize();
        HelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile);
        void draw(int& first_y);
};

#endif
