#include "stdafx.h"
#include "custom_key.h"

#include <item.h>
#include <ui.h>
#include <actors\actor.h>
#include <game.h>
#include "tile.h"
#include "actors\Person.h"
#include "spells.h"
#include "item.h"

CustomKey::CustomKey()
{
    this->init();
};

CustomKey::CustomKey(int index)
{ 
    this->init();
    this->index = index;
};

void CustomKey::init()
{
    // this->element = NULL; //needs to happen to init to null, so that reset_state's element check won't fail
    // this->reset_state();
    this->element = NULL;
    this->is_spell = false;
    this->is_item = false;

    this->index = -1;

};

void CustomKey::reset_state()
{
    if (this->is_spell)
    {
        Spell* spell = (Spell*)this->element;
        spell->custom_key = NULL;
    }
    else if (this->is_item)
    {
        Item* item = (Item*)this->element;
        item->custom_key = NULL;
    };
    this->element = NULL;
    this->is_spell = false;
    this->is_item = false;

};

void CustomKey::assign_spell(Spell* spell)
{
    this->reset_state();
    this->is_spell = true;
    this->element = spell;
    spell->custom_key = this;
};

void CustomKey::assign_item(Item* item)
{
    if (item->spell_effect == NULL) { return; };
    this->reset_state();
    this->is_item = true;
    this->element = item;
    item->spell_effect->custom_key = this;
};

std::string CustomKey::get_element_name()
{
    if (!this->is_bound()) { return ""; };

    if (this->is_spell)
    {
        Spell* spell = static_cast<Spell*>(this->element);
        return spell->name;
    }
    else if (this->is_item)
    {
        Item* item = static_cast<Item*>(this->element);
        return item->name;

    }
    else
    {
        return "Unknown element type";
    };

};

bool CustomKey::is_bound()
{
    return this->element != NULL;
};

void CustomKey::activate()
{

    if (this->is_spell)
    {
        Ui::generic_active = true;
        Ui::chosen_generic = this->element;
        Ui::is_targetting = true;
        Ui::targetted_tile = Game::player->my_tile;
    }
    else if (this->is_item)
    {
        Item* item = static_cast<Item*>(this->element);
        if (item->usable)
        {
            item->use((Actor*)Game::player);
        }
        else if (item->spell_effect != NULL)
        {
            Ui::generic_active = true;
            Ui::chosen_generic = item->spell_effect;
            Ui::is_targetting = true;
            Ui::targetted_tile = Game::player->my_tile;
        };
    };
};

