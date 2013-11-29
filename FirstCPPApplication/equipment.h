#pragma once 
#ifndef EQUIPMENT_H
#define EQUIPMENT_H


class Actor;
class Person;
class Item;
class Inventory;
class Tile;


enum slots_t : unsigned int {
    Head, 
    Ear,
    Neck,
    Chest,
    Shoulders,
    Forearms,
    Hands,
    Fingers,
    MainHand,
    OffHand
};


class Slot
{
    slots_t type;

    public:
        Slot(slots_t type);
        void Update();

        bool CanFitInSlot(Item* item);
        void AddToSlot(Item* item);
        void RemoveFromSlot(Item* item);


};
class Equipment
{

    public:
        //apply buffs/debuffs and durability for all equipment
        ~Equipment();
        Equipment();
        void Update();

        Slot* head;
        Slot* earrings;
        Slot* necklace;

        Slot* chest;
        Slot* left_shoulder;
        Slot* right_shoulder;

        Slot* left_bracer;
        Slot* right_bracer;

        Slot* left_glove;
        Slot* right_glove;

        Slot* left_ring_ring;
        Slot* right_ring_ring;
        Slot* left_ring_middle;
        Slot* right_ring_middle;
        Slot* left_ring_index;
        Slot* right_ring_index;

        Slot* main_weapon;
        Slot* off_weapon;

};


#endif
