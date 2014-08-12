#pragma once
#ifndef BACKGROUND_INFO_H
#define BACKGROUND_INFO_H

#include "enums\background_types.h"
class DialogHelpBox;
class BackgroundInfo;

class Background
{
    public:
        BackgroundInfo* gender;
        BackgroundInfo* race;
        BackgroundInfo* hometown;

        Background()
        {
            this->gender = NULL;
            this->race = NULL;
            this->hometown = NULL;
        };

};

class BackgroundInfo
{
    public:
        BackgroundInfo();

        background_types_t type;
        void activate();
        void examine();

        std::string title;
        std::string description;

        std::vector<std::string>* left_box_text;
        std::vector<std::string>* mid_box_text;
        std::vector<std::string>* right_box_text;

        int get_windows_num(); //returns amount of alert windows to show on examine
        std::vector<DialogHelpBox*>* create_alerts();
        void position_alerts(std::vector<DialogHelpBox*>* alerts);
        void position_1_alerts(std::vector<DialogHelpBox*>* alerts);
};

#endif
