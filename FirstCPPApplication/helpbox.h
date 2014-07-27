#pragma once
#ifndef HELPBOX_H
#define HELPBOX_H

#include "enums/screens.h"

class Tile;


class HelpBox 
{
    public:
        std::vector<std::string> messages;
        int x, y;
        int height, width;
        int extra_padding;
        int left_pad, right_pad, top_pad, bot_pad;
        Tile* selected_tile;
        TCODConsole* con;
        TCODConsole* target_con;

        Screens return_screen;

        HelpBox();
        ~HelpBox();
        HelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile);

        void initialize();
        void draw();
        void draw(int& first_x, int& first_y);
        virtual void set_background();
        virtual void draw_outline();
        virtual void draw_image();
        virtual void draw_messages();
};

class TargetHelpBox : public HelpBox
{
    public:
        TargetHelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile) ;

};

class DialogHelpBox : public HelpBox
{
    public:
        const void default_accept();
        const void default_cancel();


        DialogHelpBox(std::vector<std::string> messages, void (*on_accept)(), void (*on_cancel)(), TCODConsole* target_con) ;
        DialogHelpBox(std::vector<std::string> messages, TCODConsole* target_con) ;

        void (*on_accept)();
        void (*on_cancel)();
        void accept();
        void cancel();

};

#endif
