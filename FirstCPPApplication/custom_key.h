#pragma once
#ifndef CUSTOM_KEY_H
#define CUSTOM_KEY_H


class Spell;
class Item;

class CustomKey 
{
    bool is_spell;
    bool is_item;

    void* element;

    int index;

    CustomKey();

    void activate();

    void reset_state();

    void assign_spell(Spell* spell);
    void assign_item(Item* item);
};

#endif
