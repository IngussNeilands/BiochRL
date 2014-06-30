#include "stdafx.h"
#include <math.h>

#include "game.h"
#include "actors\actor.h"
#include "actors\Person.h"
#include "combat.h"
#include "spells.h"
#include "attribute_container.h"
#include "attribute.h"
#include "attr_effect.h"
#include "tile.h"
#include "ui.h"
#include "messages.h"
#include "item.h"
#include "inventory.h"
#include "enemies\skeleton.h"
#include "map.h"
#include <thinker.h>


Spell::Spell()
{
    this->name = "Unamed spell";
    this->mana_cost = 10;


    this->turn_cooldown = 1;
    this->max_range = 7;
    this->min_range = 1;
    this->min_char_level = 1;
    this->cast_count = 0;

    this->required_level = 1;

    this->attrs = new AttributeContainer;
    // this->attrs->health->current_val = 100;

    this->attr_effect = new AttrEffect;
    this->attr_effect->set_all_vals_to(0);

    this->element = elements_t::FireElement;

    this->aoe = 0;
    this->target_type = TargetTypes::TargettedTargetType;

    this->custom_key = NULL;

    this->master = NULL;

};

bool Spell::is_valid_target(Tile* targetted_tile)
{
    if (Ui::is_targetting && targetted_tile->is_occupied() || this->target_type == GroundTargetType)
    {
        return true;
    }
    else
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Pick an actual target how about.");
        return false;
    };
};

bool Spell::is_in_range(int distance) 
{
    if (distance < this->max_range)
    {
        return true;
    }
    else
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Out of range. Max is %i, you're at %i.", this->max_range, distance);
        return false;
    };
};

bool Spell::has_enough_mana() 
{ 
    if ( this->master->attrs->mana->current_val >= this->mana_cost)
    {
        return true;
    }
    else
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "No mana for this cast!");
        return false;
    };
};

TCODColor Spell::get_spell_color()
{
    std::unordered_map<elements_t, TCODColor> spell_color = std::unordered_map<elements_t, TCODColor>();

    spell_color.insert(std::make_pair<elements_t, TCODColor>(FireElement, TCODColor::red));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(WaterElement, TCODColor::blue));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(LifeElement, TCODColor::lightGreen));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(DeathElement, TCODColor::darkestGrey));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(SpectreElement, TCODColor::darkPurple));
    spell_color.insert(std::make_pair<elements_t, TCODColor>(CrystalElement, TCODColor::magenta));

    return spell_color.at(this->element);
};

void Spell::cast(Tile* targetted_tile)
{
    this->cast_count += 1;
    actor_vec_t targets = this->targets_around_tile(targetted_tile);
    typedef actor_vec_t actor_vector;
    for (actor_vector::iterator it = targets.begin(); it != targets.end(); it++)
    {
        Actor* target = *it;
        this->apply_attr_effects(target);
        Game::player->combat->last_victim = target;
        target->combat->RememberAttacker(Game::player->combat, true);
        //Game::player->combat->Attack(target->combat, 0); //HACK to get exp and printout from casting
    };

    if (this->master == Game::player)
    {
        Game::stats->spells_cast++;
    };
    this->master->attrs->mana->current_val -= mana_cost;
};

void Spell::apply_attr_effects(Actor* target)
{
    this->attr_effect->ApplyAllEffects(target);
    if (target->combat != NULL)
    {
        this->master->combat->Attack(target->combat, this->attr_effect->damage);
    };

    if (this->attr_effect->duration > 0)
    {
        TimedEffect* timed_effect = new TimedEffect;
        timed_effect->effect = this->attr_effect;
        timed_effect->turn_applied = Game::turn_count;
        target->timed_spell_effects->push_back(timed_effect);
    };

};


actor_vec_t Spell::targets_around_tile(Tile* target_tile)
{
    typedef tile_vec_t tile_vector;
    actor_vec_t targets;
    if (this->aoe > 0)
    {
        //if (this->aoe != 1) printf("aoe is only directly adjacent tiles, anything higher than 1 is ignored\n");
        tile_vec_t* adjacent_tiles = target_tile->getAdjacentTiles(this->aoe);
        for (tile_vector::iterator it = adjacent_tiles->begin(); it != adjacent_tiles->end(); it++)
        {
			Tile* tile = *it;
            if ((*it)->is_occupied())
            {
                targets.push_back((*it)->occupant);
            }
        };
        delete adjacent_tiles;
    };
    if (target_tile->occupant != NULL) // assuming NULL if they died
    {
        targets.push_back(target_tile->occupant);
    }

    return targets;

};

/* fighter */

WaterBombSpell::WaterBombSpell() : Spell()
{
    this->name = "Water Bomb";
    this->element = WaterElement;
    this->attr_effect->damage->water = 7;
    this->mana_cost = 5;
    this->max_range = 11;
};

AutoChemHPSpell::AutoChemHPSpell() : Spell()
{
    this->name = "AutoChemistry: Health Potion";
    this->element = LifeElement;
    this->attr_effect->health_current_val = 15;
    this->mana_cost = 5;
    this->max_range = 1;

    this->required_level = 4;
};

PoisonCoughSpell::PoisonCoughSpell() : Spell()
{
    this->name = "Poison Cough";
    this->element = DeathElement;
    this->attr_effect->damage->death = 25;
    this->mana_cost = 20;
    this->max_range = 2;
    this->required_level = 6;
};

InnerSanctuarySpell::InnerSanctuarySpell() : Spell()
{
    this->required_level = 8;
    this->name = "Inner Sanctuary";
    this->element = LifeElement;
    this->attr_effect->health_max_val = 30;
    this->mana_cost = 20;
    this->max_range = 1;
    this->attr_effect->duration = 50;
};

/* mage */

IceBoltSpell::IceBoltSpell() : Spell()
{

    this->required_level = 2;
    this->name = "Ice bolt";
    this->element = WaterElement;
    this->attr_effect->damage->water = 9;
    this->mana_cost = 7;
    this->max_range = 11;
};

InnerHealingSpiritSpell::InnerHealingSpiritSpell() : Spell()
{

    this->required_level = 4;
    this->name = "Inner Healing Spirit";
    this->element = LifeElement;
    this->attr_effect->health_current_val = 15;
    this->mana_cost = 5;
    this->max_range = 1;
};

DeathsTouchSpell::DeathsTouchSpell() : Spell()
{

    this->required_level = 6;
    this->name = "Death's Touch";
    this->element = DeathElement;
    this->attr_effect->damage->death = 25;
    this->mana_cost = 20;
    this->max_range = 2;
};

InnerFountainSpell::InnerFountainSpell() : Spell()
{

    this->required_level = 8;
    this->name = "Inner Fountain";
    this->element = CrystalElement;
    this->attr_effect->mana_regen_rate = 5;
    this->mana_cost = 20;
    this->max_range = 1;
    this->attr_effect->duration = 50;
};

/* necromancy */

CorpseBlastSpell::CorpseBlastSpell() : Spell()
{

    this->required_level = 2;
    this->aoe = 1;
    this->target_type = GroundTargetType;
    this->name = "Corpse Blast";
    this->element = DeathElement;
    this->attr_effect->damage->death = 15;
    this->mana_cost = 7;
    this->max_range = 11;
};

void CorpseBlastSpell::cast(Tile* targetted_tile)
{
    //check for corpse
    std::vector<Item*>* items = targetted_tile->inventory->items;
    if (items->empty()) { return; };
    auto it = items->begin();
    bool found_corpse = false;
    std::string corpse = "corpse";
    for (it; it != items->end(); it++)
    {
        std::size_t found = (*it)->name.find(corpse);
        if (found != std::string::npos)
        {
            found_corpse = true;
            break;
        }
    }

    //cast spell, apply attrs etc
    if (found_corpse)
    {
        Spell::cast(targetted_tile);
    };
};

SiphonSpiritSpell::SiphonSpiritSpell() : Spell()
{

    this->required_level = 4;
    this->name = "Siphon Spirit";
    this->element = DeathElement;
    this->attr_effect->damage->death = 7;
    this->mana_cost = 15;
    this->max_range = 2;
};

void SiphonSpiritSpell::apply_attr_effects(Actor* target)
{
    Spell::apply_attr_effects(target);
    int val  = std::abs(this->attr_effect->damage->death);
    this->master->attrs->health->current_val += val;

    //take from target and give to caster
};

RaiseDeadSpell::RaiseDeadSpell() : Spell()
{

    this->required_level = 6;
    this->name = "Raise the dead";
    this->element = DeathElement;
    // this->attr_effect->damage->normal = 25;
    this->mana_cost = 20;
    this->max_range = 5;
    this->target_type = GroundTargetType;
};

void RaiseDeadSpell::raise_dead(Tile* targetted_tile)
{
    Game::spawn_creature_ally<Skeleton>(targetted_tile, "Skellie", 1000, 'S', Game::current_map);
};

void RaiseDeadSpell::cast(Tile* targetted_tile)
{
    //check for corpse
    std::vector<Item*>* items = targetted_tile->inventory->items;
    if (items->empty() || targetted_tile->is_occupied()) { return; };
    auto it = items->begin();
    bool found_corpse = false;
    std::string corpse = "corpse";
    for (it; it != items->end(); it++)
    {
        std::size_t found = (*it)->name.find(corpse);
        if (found != std::string::npos)
        {
            found_corpse = true;
            break;
        }
    }

    //cast spell, apply attrs etc
    if (found_corpse)
    {
        this->raise_dead(targetted_tile);
        if (this->master == Game::player)
        {
            Game::stats->spells_cast++;
        };
        this->master->attrs->mana->current_val -= this->mana_cost;
        // Spell::cast(targetted_tile);
    };
};

InnerFireSpell::InnerFireSpell() : Spell()
{

    this->required_level = 8;
    this->name = "Inner Fire";
    this->element = FireElement;
    this->attr_effect->mana_regen_rate = 5;
    this->mana_cost = 20;
    this->max_range = 1;
    this->attr_effect->duration = 50;
};


/* brawler */

IceFistSpell::IceFistSpell() : Spell()
{

    this->required_level = 2;
    this->name = "Ice fist";
    this->element = WaterElement;
    this->attr_effect->damage->water = 7;
    this->mana_cost = 5;
    this->max_range = 2;
};

SacredKickSpell::SacredKickSpell() : Spell()
{

    this->required_level = 4;
    this->name = "Sacred Kick";
    this->element = LifeElement;
    this->attr_effect->damage->life = 15;
    this->mana_cost = 5;
    this->max_range = 3;
};

DeathsHandSpell::DeathsHandSpell() : Spell()
{

    this->required_level = 6;
    this->name = "Death's Hand";
    this->element = DeathElement;
    this->attr_effect->damage->death = 25;
    this->mana_cost = 20;
    this->max_range = 2;
};

InnerStrengthSpell::InnerStrengthSpell() : Spell()
{

    this->required_level = 8;
    this->name = "Inner Strength";
    this->element = DeathElement;
    this->attr_effect->damage_current_val = 15;
    this->mana_cost = 20;
    this->max_range = 1;
    this->attr_effect->duration = 23;
};

/* Shadower */
ShadowRunSpell::ShadowRunSpell() : TeleportSelfSpell()
{

    this->required_level = 2;
    this->name = "Shadow Run";
    this->element = SpectreElement;
    this->mana_cost = 3;
    this->max_range = 4;
    this->target_type = GroundTargetType;
};

CastShadowSpell::CastShadowSpell() : Spell()
{
    this->required_level = 4;
    this->name = "Cast Shadows";
    this->element = SpectreElement;
    this->mana_cost = 15;
    this->max_range = 2;
    this->aoe = 2;
    this->target_type = GroundTargetType;
};

void CastShadowSpell::cast(Tile* targetted_tile)
{
    //get tiles within a given radius
    tile_vec_t* tiles = targetted_tile->getAdjacentTiles(this->aoe);
    //
    //mark occupants unaware
    tile_vec_t::iterator it = tiles->begin();
    for (it; it != tiles->end(); it++)
    {
        Tile* tile = *it;
        if (tile->is_occupied() && tile->occupant != this->master && tile->occupant->thinker != NULL)
        {
            tile->occupant->thinker->set_aware(false);
        };
    };
    this->master->attrs->mana->current_val -= mana_cost;

    //
    //auto sneak?
};

BribeSpell::BribeSpell() : Spell()
{
    this->required_level = 6;
    this->name = "Bribery";
    this->element = SpectreElement;
    this->mana_cost = 15;
    this->max_range = 4;
    this->aoe = 0;
    this->target_type = TargettedTargetType;
};

void BribeSpell::cast(Tile* targetted_tile)
{
    //get tiles within a given radius
    tile_vec_t* tiles = targetted_tile->getAdjacentTiles(this->aoe);

    tile_vec_t::iterator it = tiles->begin();
    for (it; it != tiles->end(); it++)
    {
        Tile* tile = *it;
        if (tile->is_occupied() && tile->occupant != this->master && tile->occupant->thinker != NULL)
        {
            tile->occupant->thinker->is_ally = true;
        };
    };
    this->master->attrs->mana->current_val -= mana_cost;

};

/* misc */

TeleportSelfSpell::TeleportSelfSpell() : Spell()
{

    this->required_level = 8;
    this->name = "Translocation";
    this->element = SpectreElement;
    this->mana_cost = 25;
    this->max_range = 6;
    this->target_type = GroundTargetType;
};

void TeleportSelfSpell::cast(Tile* targetted_tile)
{
    if (targetted_tile->is_occupied()) { return; };

    //cast spell, apply attrs etc
    if (targetted_tile->is_walkable())
    {
        this->master->putPerson(targetted_tile, targetted_tile->tile_x, targetted_tile->tile_y);
        if (this->master == Game::player)
        {
            Game::stats->spells_cast++;
        };
        this->master->attrs->mana->current_val -= mana_cost;
        // Spell::cast(targetted_tile);
    }
    else
    {
        printf("Is not walkable\n");
    }
};

FireBallSpell::FireBallSpell() : Spell()
{
    this->required_level = 1;
    this->name = "Fireball";
    this->element = FireElement;
    this->attr_effect->damage->fire = 10;
    this->mana_cost = 10;
    this->max_range = 10;
    this->aoe = 1;
    this->target_type = GroundTargetType;
};

LaunchOtherSpell::LaunchOtherSpell() : Spell()
{

    this->required_level = 8;
    this->name = "Launch Other";
    this->element = CrystalElement;
    this->mana_cost = 10;
    this->max_range = 4;
    this->force = 2;
    // this->target_type = GroundTargetType;
};

void LaunchOtherSpell::cast(Tile* targetted_tile)
{
    //make sure someone is there
    if (!targetted_tile->is_occupied()) { return; };

    //cast spell, apply attrs etc

    //if (targetted_tile->is_walkable())
    if (true)
    {
        //get angle
        int x1, y1, x2, y2;
        x1 = this->master->my_tile->tile_x;
        y1 = this->master->my_tile->tile_y;
        x2 = targetted_tile->occupant->my_tile->tile_x;
        y2 = targetted_tile->occupant->my_tile->tile_y;

        double delta_x = x2-x1;
        double delta_y = y2-y1;

        int new_x, new_y;
        if (delta_x < 0) { new_x = delta_x-this->force; }
        else if (delta_x > 0) { new_x = delta_x+this->force; }
        else { new_x = delta_x; };

        if (delta_y < 0) { new_y = delta_y-this->force; }
        else if (delta_y > 0) { new_y = delta_y+this->force; }
        else { new_y = delta_y; }


        //push target along that angle
        Tile* new_tile = targetted_tile->getTileAtRelative(new_x, new_y);
        //FIXME ai pathing puts them back on their path, otherwise
        if (targetted_tile->occupant->l_path != NULL)
        {
            delete targetted_tile->occupant->l_path;
            targetted_tile->occupant->l_path = NULL;
        };
        targetted_tile->occupant->putPerson(new_tile, new_tile->tile_x, new_tile->tile_y);
        if (this->master == Game::player)
        {
            Game::stats->spells_cast++;
        };
        this->master->attrs->mana->current_val -= mana_cost;
        // Spell::cast(targetted_tile);
    }
    else
    {
        printf("Is not walkable\n");
    }

};
