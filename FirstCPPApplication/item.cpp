
#include "stdafx.h"
#include "item.h"
#include "game.h"
#include "Representation.h"
#include "attr_effect.h"
#include "actors/actor.h"
#include "tile.h"
#include "inventory.h"
#include <custom_key.h>
#include <spells.h>

Item::Item()
{
    this->repr = new BaseItemRepresentation();
    this->slot_type =  slots_t::Chest;
    this->attr_effect = new AttrEffect;
    this->weight = 1;

    this->name = "A Nameless Item";
    this->description = "A Descriptionless Item.";

    this->spell_effect = NULL;
    this->custom_key = NULL;

    this->usable = false;
    this->uses = 1;
    this->equippable = false;
};

Item::~Item()
{
    delete this->repr;
    delete this->attr_effect;

    if (this->spell_effect != NULL)
    {
        delete this->spell_effect;
    };

    if (this->custom_key != NULL)
    {
        this->custom_key->reset_state();
    };
};


void Item::use(Actor* target)
{
    if ( this->usable )
    {
        if (target == (Actor*)Game::player) { Game::stats->items_used++; };
        this->attr_effect->ApplyAllEffects(target);
        this->uses--;
        if (this->attr_effect->is_timed())
        {
            TimedEffect* timed_effect = new TimedEffect;
            timed_effect->effect = this->attr_effect;
            timed_effect->turn_applied = Game::turn_count;
            target->timed_attr_effects->push_back(timed_effect);
        };
        if (this->uses <= 0)
        {
            target->inventory->remove_item(this);
            // delete this; //dont want to delete it if its a timed item
        }
    }
    else
    {
        std::cout << "Not a usable item, try equipping it" << std::endl;
    };
};

void Item::set_and_name_for_dmg(std::string pre_name, std::string post_name, TCODRandom* rng, int min, int max, int med)
{

    int val = this->attr_effect->set_rng_damage(rng, min, max, med);
    float percent = (float(val))/ ((float)max) * 100;
    std::string mid_name = "";
    if (percent < 10.0f) { mid_name = "broken";}
    else if (percent < 15.0f) { mid_name = "crude";}
    else if (percent < 20.0f) { mid_name = "rusted";}
    else if (percent < 25.0f) { mid_name = "dented";}
    else if (percent < 30.0f) { mid_name = "bent";}
    else if (percent < 35.0f) { mid_name = "unrefined";}
    else if (percent < 40.0f) { mid_name = "weakened";}
    else if (percent < 45.0f) { mid_name = "poor";}
    else if (percent < 50.0f) { mid_name = "ordinary";}
    else if (percent < 60.0f) { mid_name = "average";}
    else if (percent < 65.0f) { mid_name = "modest";}
    else if (percent < 70.0f) { mid_name = "polished";}
    else if (percent < 75.0f) { mid_name = "average";}
    else if (percent < 80.0f) { mid_name = "rugged";}
    else if (percent < 85.0f) { mid_name = "polished";}
    else if (percent < 90.0f) { mid_name = "handcrafted";}
    else if (percent < 95.0f) { mid_name = "engraved";}
    else  { mid_name = "adorned";};
    std::stringstream name;
    name << pre_name << " " << mid_name << " " << post_name;
    this->name = colfg(TCODColor::lightestRed, name.str());
};

void Item::set_and_name_for_arm(std::string pre_name, std::string post_name, TCODRandom* rng, int min, int max, int med)
{

    int val = this->attr_effect->set_rng_armor(rng, min, max, med);
    float percent = (float(val))/ ((float)max) * 100;
    std::string mid_name = "";
    if (percent < 10.0f) { mid_name = "broken";}
    else if (percent < 15.0f) { mid_name = "crude";}
    else if (percent < 20.0f) { mid_name = "rusted";}
    else if (percent < 25.0f) { mid_name = "dented";}
    else if (percent < 30.0f) { mid_name = "bent";}
    else if (percent < 35.0f) { mid_name = "unrefined";}
    else if (percent < 40.0f) { mid_name = "weakened";}
    else if (percent < 45.0f) { mid_name = "poor";}
    else if (percent < 50.0f) { mid_name = "ordinary";}
    else if (percent < 60.0f) { mid_name = "average";}
    else if (percent < 65.0f) { mid_name = "modest";}
    else if (percent < 70.0f) { mid_name = "polished";}
    else if (percent < 75.0f) { mid_name = "average";}
    else if (percent < 80.0f) { mid_name = "rugged";}
    else if (percent < 85.0f) { mid_name = "polished";}
    else if (percent < 90.0f) { mid_name = "handcrafted";}
    else if (percent < 95.0f) { mid_name = "engraved";}
    else  { mid_name = "adorned";};
    std::stringstream name;
    name << pre_name << " " << mid_name << " " << post_name;
    this->name = colfg(TCODColor::lightestBlue, name.str());
};

void Item::unequip(Actor* target)
{
    if ( this->equippable )
    {
        //this->attr_effect->RemoveAllEffects(target);
    }
    else
    {
        std::cout << "Not a unequippable item" << std::endl;
    };
};

void Item::equip(Actor* target)
{
    if ( this->equippable )
    {
        //this doesn't happen here any more
        // this->attr_effect->ApplyAllEffects(target);
    }
    else
    {
        std::cout << "Not a equippable item, try using it" << std::endl;
    };
};
