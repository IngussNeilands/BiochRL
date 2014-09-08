#pragma once
#ifndef CIVILIAN_H
#define CIVILIAN_H
#include <vector>
#include <string>


class Actor;

class Civilian
{
    public:
        int disposition;
        bool is_shopkeep;
        bool is_weaponsmith;

        Actor* master;
        std::vector<std::string>* chat_lines;

        Civilian();
        ~Civilian();
        std::string talk_general_topic();
        void start_combat(){}; //once its fought create a combat for this->master

        unsigned long long sell_from_floor(Actor* client);
        int upgrade_primary_weapon(Actor* client);
};
#endif
