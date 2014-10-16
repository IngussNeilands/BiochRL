#pragma once
#ifndef CLASS_H
#define CLASS_H

class Actor;
class Spell;
class Attribute;
class AttributeContainer;

enum ClassType
{
    FighterClassType = 1,
    StalkerClassType = 2,
    MageClassType = 3,
    NecromancerClassType = 4,
    BrawlerClassType = 5,
    BloodMageClassType = 6,
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

        AttributeContainer* starting_attrs;

        std::map<int, Spell*>* spell_map;

        TCODColor fg_color;

        IClass();
        virtual ~IClass() {};

        virtual void apply_starting_stats();

        virtual void LevelUpStats(int levels) = 0;
        virtual void LevelUpSkills(int levels);

        double health_on_lvl;
        double mana_on_lvl;
        double armor_on_lvl;
        double damage_on_lvl;

        void LevelUpArmor();
        void LevelUpArmor(double change);
        void LevelUpDamage();
        void LevelUpDamage(double change);
        void LevelUpHealth();
        void LevelUpHealth(double change);
        void LevelUpMana();
        void LevelUpMana(double change);
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

class BloodMageClass : public IClass
{
    public:
        BloodMageClass();
        virtual void LevelUpStats(int levels);
};


#endif
