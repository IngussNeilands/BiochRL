#pragma once
#ifndef GODS_H
#define GODS_H

class God
{
    public:
        std::string name;

        void pray_to(Actor* worshipper);
        int get_favor(Actor* worshipper);

        God();

};


#endif
