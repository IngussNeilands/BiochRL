#pragma once
#ifndef MENU_ITEM_H
#define MENU_ITEM_H

class Game;
class Actor;
class ScreenItem;
class Ui;
class BackgroundInfo;


/*
   for simple selection menu items, like yes or no, or race, gender etc
   for picking things, basically without much special logic
   */
class MenuItem
{
    public:
        MenuItem();
        void init();

        std::string get_title();
        std::string get_description();


        bool (*on_activate)();
		BackgroundInfo* info; //has the activate/examine logic inside

        void activate(); //applies to player or whatever action is appropriate
        void examine(); //shows the alert pages, like class info or whatever

};

#endif
