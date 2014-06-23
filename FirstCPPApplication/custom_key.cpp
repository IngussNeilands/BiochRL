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
    // this->element = NULL; //needs to happen to init to null, so that reset_state's element check won't fail
    // this->reset_state();
    this->element = NULL;
    this->is_spell = false;
    this->is_item = false;
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
    this->reset_state();
    this->is_item = true;
    this->element = item;
    item->custom_key = this;
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

