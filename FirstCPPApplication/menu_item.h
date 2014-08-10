#pragma once
#ifndef MENU_ITEM_H
#define MENU_ITEM_H

class Screen;
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

        std::string title;
        std::string description;

        str_vec_t* left_box_text;
        str_vec_t* mid_box_text;
        str_vec_t* right_box_text;

        void activate(); //applies to player or whatever action is appropriate

        void examine(); //shows the alert pages, like class info or whatever
        int get_windows_num(); //returns amount of alert windows to show on examine

};

#endif
