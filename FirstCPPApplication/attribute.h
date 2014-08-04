#ifndef ATTRBUTE_H
#define ATTRBUTE_H

class Actor;
class Person;
class Combat;
class Person;
class Item;
class Inventory;
class Tile;

class Attribute 
{
    private:
        int tick;
        bool is_ready;

    public:
        double current_val;
        double max_val;

        double regen_rate; //per turn
        double regen_interval; //per x turns? 

        Attribute();
        Attribute(Attribute& other);

        static const TCODColor attribute_color;


        virtual void Update();
        virtual void Tick();
        virtual bool CheckIsReady();

        void SetVals(int both);
        void SetVals(int current_val, int max_val);
        void AlterVals(int both);
        void AlterVals(int diff_current, int diff_max);

        // bool CanRegenerate();
        void Regenerate();
        
        float GetValPercentage();

        virtual void AddToCurrentVal(int difference);
        virtual void AddToMaxVal(int difference);
        virtual void AddToRegenRate(int difference);
        virtual void AddToRegenInterval(int difference);

        virtual void RemoveFromCurrentVal(int difference);
        virtual void RemoveFromMaxVal(int difference);
        virtual void RemoveFromRegenRate(int difference);
        virtual void RemoveFromRegenInterval(int difference);

};

class HealthAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        HealthAttribute();
	//void Regenerate();
};

class ManaAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        ManaAttribute();
};

class ArmorAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        ArmorAttribute();
};

class DamageAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        DamageAttribute();
};

class HungerAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        HungerAttribute();
};

class SpeedAttribute : public Attribute
{
    public:
        static const TCODColor attribute_color;
        SpeedAttribute();
};

#endif
