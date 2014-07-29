#pragma once
#ifndef CUSTOM_KEY_H
#define CUSTOM_KEY_H


class Spell;
class Item;

class CustomKey 
{
    public:
        bool is_spell;
        bool is_item;

        void* element;

        int index;

        CustomKey();
        CustomKey(int index);

        void init();
        void activate();

        void reset_state();
        bool is_bound();
        std::string get_element_name();

        void assign_spell(Spell* spell);
        void assign_item(Item* item);
};

#endif
