#pragma once
#ifndef TOWNSMEN_H
#define TOWNSMEN_H

class Townsmen
{
    public:
        Actor* master;

        Townsmen();
        ~Townsmen();

        void update();
};
#endif
