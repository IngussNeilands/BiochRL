#ifndef ITEM_H
#define ITEM_H

#include <string>

// #include "equipment.h"
#include <enums\slots_t.h>

class Representation;
class Slot;
class AttrEffect;
class Actor;
class Spell;
class CustomKey;

// enum slots_t : unsigned int;

class Item
{
    public:
        std::string description;
        std::string name;

        int weight;

        bool usable;
        int uses;
        bool equippable;

        int range;

        Spell* spell_effect;

        AttrEffect* attr_effect;
        Representation* repr;

        CustomKey* custom_key;

        Item();
        ~Item();
        void use(Actor* target);
        void equip(Actor* target);
        void unequip(Actor* target);

        void set_and_name_for_dmg(std::string pre_name, std::string post_name, TCODRandom* rng, int min, int max, int med);
        void set_and_name_for_arm(std::string pre_name, std::string post_name, TCODRandom* rng, int min, int max, int med);

        slots_t slot_type;

};

#endif
