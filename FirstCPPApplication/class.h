#pragma once
#ifndef CLASS_H
#define CLASS_H

class Actor;
class Spell;



enum ClassType
{
    FighterClassType = 1,
    StalkerClassType = 2,
    MageClassType = 3,
    NecromancerClassType = 4,
    BrawlerClassType = 5,
    NoClassType = -1
};

#include <map>

class IClass
{
    public:
        ClassType type;
        Actor* master;
        std::string name;
        std::string description;
        std::string levelup_message;

        std::map<int, Spell*>* spell_map;

        TCODColor fg_color;

        IClass() { this->type = NoClassType;};
        virtual ~IClass() {};

        virtual void LevelUpStats(int levels) = 0;
        virtual void LevelUpSkills(int levels);

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
};

class StalkerClass : public IClass
{
    public:
        StalkerClass();
        virtual void LevelUpStats(int levels);
};

class MageClass : public IClass
{
    public:
        MageClass();
        virtual void LevelUpStats(int levels);
};

class NecromancerClass : public IClass
{
    public:
        NecromancerClass();
        virtual void LevelUpStats(int levels);
};

class BrawlerClass : public IClass
{
    public:
        BrawlerClass();
        virtual void LevelUpStats(int levels);
};


#endif
