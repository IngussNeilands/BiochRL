#pragma once
#ifndef GODS_H
#define GODS_H

class God
{
    public:
        std::string name;

        virtual int get_favor(Actor* worshipper) = 0;
        virtual int_str_map_t get_favour_threshold(Actor* worshipper);

        void pray_to(Actor* worshipper);

        God();

};

class AugustineGod : public God 
{
    public:
        AugustineGod();
        virtual int get_favor(Actor* worshipper);
};

class EpiphneGod : public God 
{
    public:
        EpiphneGod();
        virtual int get_favor(Actor* worshipper);
};


class TheTransientGod : public God 
{
    public:
        TheTransientGod();
        virtual int get_favor(Actor* worshipper);
};


#endif
