#pragma once
#ifndef CLASS_H
#define CLASS_H

class Actor;

class IClass
{
    public:
          int type;
        Actor* master;
        std::string name;
        std::string description;

        TCODColor fg_color;

        IClass() { this->type = -1;};
        virtual ~IClass() {};

        virtual void LevelUpStats(int levels) = 0;
        virtual void LevelUpSkills(int levels) = 0;

        void LevelUpHealth(double change);
        void LevelUpMana(double change);
        void LevelUpArmor(double change);
        void LevelUpDamage(double change);
};

class FighterClass : public IClass
{
    public:
        FighterClass();
        virtual void LevelUpStats(int levels);
        virtual void LevelUpSkills(int levels);
};

class MageClass : public IClass
{
    public:
        MageClass();
        virtual void LevelUpStats(int levels);
        virtual void LevelUpSkills(int levels);
};

class NecromancerClass : public IClass
{
    public:
        NecromancerClass();
        virtual void LevelUpStats(int levels);
        virtual void LevelUpSkills(int levels);
};

class BrawlerClass : public IClass
{
    public:
        BrawlerClass();
        virtual void LevelUpStats(int levels);
        virtual void LevelUpSkills(int levels);
};


#endif
