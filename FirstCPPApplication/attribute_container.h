#pragma once
#ifndef ATTRIBUTE_CONTAINER_H
#define ATTRIBUTE_CONTAINER_H


#include <string>
#include <vector>

// class Representation;
// class Tile;
// class Thinker;
// class Game;
// class Inventory;
// class Equipment;
// class Item;
class HealthAttribute;
class ManaAttribute;
class ArmorAttribute;
class DamageAttribute;
class HungerAttribute;
class Actor;

class AttributeContainer
{
    public:
        Actor* owner;

        HealthAttribute* health;
        ManaAttribute* mana;
        ArmorAttribute* armor;
        DamageAttribute* damage;
        HungerAttribute* hunger;

        AttributeContainer();
        AttributeContainer(AttributeContainer& other);

        void RegenerateAll();
        void RegenerateHealth();
        void RegenerateMana();
        void RegenerateArmor();
        void RegenerateDamage();

        void Update();
        std::string PrettyPrint();
		std::vector<std::string> AttributeContainer::PrettyVector();

};

#endif
