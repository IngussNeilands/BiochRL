#pragma once 
#ifndef EQUIPMENT_H
#define EQUIPMENT_H


class Actor;
class Person;
class Item;
class Inventory;
class Tile;
class vector;

class Equipment;

#include <enums\slots_t.h>
#include <vector>


class Slot
{
    public:
        slots_t type;
        Item* equipped_item;
        Equipment* equipment;

        Slot(slots_t type, Equipment* equipment);
        void Update();

        bool CanFitInSlot(Item* item);
        void AddToSlot(Item* item);
        void Slot::RemoveFromSlot();
        void RemoveFromSlot(Item* item);
        Actor* get_master();

        bool HasRoomFor(Item* item);
        Item* GetEquippedItem();
        void apply_attr_effect();
        void remove_attr_effect();


};

class Equipment
{

    public:

        Actor* master;

        ~Equipment();
        Equipment();

        void Update();//apply buffs/debuffs and durability for all equipment
        bool is_item_equipped(Item* item);
        void unequip_item(Item* item);
        void equip_item(Item* item);

        std::vector<Slot*>* slots;

        Slot* get_slots_for_type(slots_t slot_type);

        item_vec_t get_items_equipped();
        spell_vec_t get_spells_attached();

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

        Slot* left_thigh;
        Slot* right_thigh;
        Slot* left_shin;
        Slot* right_shin;
        Slot* left_foot;
        Slot* right_foot;

};


#endif
