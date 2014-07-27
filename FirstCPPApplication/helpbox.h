#pragma once
#ifndef HELPBOX_H
#define HELPBOX_H

#include "enums/screens.h"

class Tile;


class HelpBox 
{
    public:
        int id;
        static int count;

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

        bool operator==(HelpBox& other);
        void initialize();
        void draw();
        void draw(int& first_x, int& first_y);
        virtual void set_background();
        virtual void draw_outline();
        virtual void draw_image();
        virtual void draw_messages();

        void resize(int width, int height);
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


        DialogHelpBox(std::vector<std::string> messages, void (*on_accept)(DialogHelpBox* dialog), void (*on_cancel)(DialogHelpBox* dialog), TCODConsole* target_con);
        DialogHelpBox(std::vector<std::string> messages, TCODConsole* target_con) ;
        ~DialogHelpBox();

        void (*on_accept)(DialogHelpBox* dialog);
        void (*on_cancel)(DialogHelpBox* dialog);
        void accept();
        void cancel();

};

#endif
