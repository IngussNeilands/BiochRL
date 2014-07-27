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


        virtual void Update();
        virtual void Tick();
        virtual bool CheckIsReady();

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
        HealthAttribute();
	//void Regenerate();
};

class ManaAttribute : public Attribute
{
    public:
        ManaAttribute();
};

class ArmorAttribute : public Attribute
{
    public:
        ArmorAttribute();
};

class DamageAttribute : public Attribute
{
    public:
        DamageAttribute();
};

class HungerAttribute : public Attribute
{
    public:
        HungerAttribute();
};

#endif
