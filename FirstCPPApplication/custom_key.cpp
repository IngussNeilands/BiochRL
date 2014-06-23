#include "stdafx.h"
#include "custom_key.h"

#include <item.h>
#include <ui.h>
#include <actors\actor.h>
#include <game.h>
#include "tile.h"
#include "actors\Person.h"

CustomKey::CustomKey()
{
    this->reset_state();
};

void CustomKey::reset_state()
{
    this->element = NULL;
    this->is_spell = false;
    this->is_item = false;
};

void CustomKey::assign_spell(Spell* spell)
{
    this->reset_state();
    this->is_spell = true;
    this->element = spell;
};
    
void CustomKey::assign_item(Item* item)
{
    this->reset_state();
    this->is_item = true;
    this->element = item;
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

