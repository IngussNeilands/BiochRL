#include "stdafx.h"
#include <math.h>
#include <assert.h>

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
#include <helpbox.h>
#include <enemies/crazedcook.h>


Spell::Spell()
{
    this->name = "Unamed spell";
    this->description = "An undescribable spell";
    this->mana_cost = 10;

    this->mana_type = ManaManaType;
    this->mana_percentage = false;

    this->buff_damage = false;


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
// 
// tile_vec_t* Spell::get_affected_tiles(Tile* targetted_tile)
// {
// 
// };

void Spell::increment_count()
{
    this->cast_count += 1;
    if (this->master == Game::player)
    {
        Game::stats->spells_cast++;
    };
};

bool Spell::check_resistances(Actor* target)
{
    TCODRandom* rng = Game::stat_rolls_rng;
    int result = rng->getInt(0, 100);
    int spell_resistance = 25; //TODO use target's spell resistance and the spells power to determine how likely it is to hit
    if (target->is_champion)
    {
        spell_resistance+=15;
    }
    else if (target->is_hero)
    {
        spell_resistance+=25;
    };

    if (result > spell_resistance)
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(TCODColor::lighterGreen, "%s affected %s.").c_str(), this->name.c_str(), target->name.c_str() );
        return true; //Spell was a success
    }
    else
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(TCODColor::lighterRed, "%s resisted %s!").c_str(), target->name.c_str(), this->name.c_str() );
        return false;
    };
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
    if (distance <= this->max_range)
    {
        return true;
    }
    else
    {
        return false;
    };
};

int Spell::get_mana_cost()
{
    if (!this->mana_percentage)
    {
        return this->mana_cost;
    }
    else 
    {
        Attribute* attr;
        if (this->mana_type == ManaManaType)
        {
            attr = this->master->attrs->mana;
        }
        else if (this->mana_type == BloodManaType)
        {
            attr = this->master->attrs->health;
        }
        int new_cost = ((float)this->mana_cost / 100) * (float)attr->max_val; 
        new_cost = std::max((long double)1.0, (long double)new_cost);
        return (int)new_cost;
    }
};

void reset_generic_wrapper(DialogHelpBox* dialog)
{
    Ui::reset_generic();
    Ui::is_targetting = false;
    dialog->default_cancel();
};

bool Spell::has_enough_mana() 
{ 
    if (this->mana_type == ManaManaType)
    {
        if ( this->master->attrs->mana->current_val >= this->get_mana_cost())
        {
            return true;
        }
    }
    else if (this->mana_type == BloodManaType)
    {
        if ( this->master->attrs->health->current_val >= this->get_mana_cost())
        {
            return true;
        }
    }

    std::vector<std::string> alert_msgs = std::vector<std::string>();
    alert_msgs.push_back("No mana for this cast!");
    alert_msgs.push_back("Drink a potion or stand your ground for a while.");
    alert_msgs.push_back(" ");
    alert_msgs.push_back("Hit N/Q to continue");

    DialogHelpBox* mana_alert = new DialogHelpBox(alert_msgs, NULL, &reset_generic_wrapper, Game::game_console);
    mana_alert->return_screen = Game::current_screen;
    mana_alert->y = 15;
    mana_alert->x = 20;
    mana_alert->resize(25, mana_alert->height);

    Ui::alerts.push_back(mana_alert);
    Game::current_screen = Screens::AlertScreenType;

    new Message(Ui::msg_handler_main, NOTYPE_MSG, "No mana for this cast!");
    return false;
};

TCODColor Spell::get_spell_color()
{
    return get_element_color(this->element);
};

void Spell::spend_mana()
{
    if (this->mana_type == ManaManaType)
    {
        this->master->attrs->mana->current_val -= this->get_mana_cost();
    }
    else if (this->mana_type == BloodManaType)
    {
        this->master->attrs->health->current_val -= this->get_mana_cost();
    }
    else
    {
        assert(false && "Unknown ManaType");
    };
};

bool Spell::cast(Tile* targetted_tile)
{
    actor_vec_t targets = this->targets_around_tile(targetted_tile);
    for (actor_vec_t::iterator it = targets.begin(); it != targets.end(); it++)
    {
        Actor* target = *it;
        if (! this->check_resistances(target)) { continue; };
        this->apply_attr_effects(target);

        if (target->combat != NULL)
        {
            Game::player->combat->last_victim = target;
            target->combat->remember_attacker(Game::player->combat, true);
        };
    };

    this->increment_count();
    this->spend_mana();

    return true;
};

void Spell::apply_attr_effects(Actor* target)
{
    std::vector<attribute_types_t>* exceptions = NULL;
    if (this->buff_damage != true)
    {
        exceptions = new std::vector<attribute_types_t>();
        exceptions->push_back(DamageAttrType);
    };
    this->attr_effect->ApplyAllEffects(target, exceptions);
	if (exceptions != NULL) { delete exceptions; }

    if (target->combat != NULL && !this->buff_damage)
    {
        this->master->combat->attack(target->combat, this->attr_effect->damage);
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
    return target_tile->getActorsAroundTile(this->aoe);
};

/* fighter */

WaterBombSpell::WaterBombSpell() : Spell()
{
    this->name = "Water Bomb";
    this->element = WaterElement;
    this->attr_effect->damage->water = 12;
    this->mana_cost = 7;
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

bool CorpseBlastSpell::cast(Tile* targetted_tile)
{
    //check for corpse
    std::vector<Item*>* items = targetted_tile->inventory->items;
    if (items->empty()) { return false; };
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
        return Spell::cast(targetted_tile);
    };

    return false;
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
    this->mana_cost = 20;
    this->max_range = 5;
    this->target_type = GroundTargetType;
};

void RaiseDeadSpell::raise_dead(Tile* targetted_tile)
{
    Game::spawn_creature_ally<Skeleton>(targetted_tile, "Skellie", 1000, 'S', Game::current_map);
};

bool RaiseDeadSpell::cast(Tile* targetted_tile)
{
    //check for corpse
    std::vector<Item*>* items = targetted_tile->inventory->items;
    if (items->empty() || targetted_tile->is_occupied()) { return false; };
    auto it = items->begin();
    bool found_corpse = false;
    std::string corpse = "corpse";
    Item* corpse_item = NULL;
    for (it; it != items->end(); it++)
    {
        std::size_t found = (*it)->name.find(corpse);
        if (found != std::string::npos)
        {
            found_corpse = true;
            corpse_item = *it;
            break;
        }
    }

    //cast spell, apply attrs etc
    if (found_corpse)
    {
        this->raise_dead(targetted_tile);
        targetted_tile->inventory->remove_item(corpse_item);
        delete corpse_item;

        this->increment_count();
        this->spend_mana();

        return true;
    };

    return false;
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
    this->buff_damage = true;
    this->attr_effect->damage->normal = 15;
    this->mana_cost = 20;
    this->max_range = 1;
    this->attr_effect->duration = 23;
};

/* Stalker */
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

bool CastShadowSpell::cast(Tile* targetted_tile)
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
            if (! this->check_resistances(tile->occupant)) { continue; };
            tile->occupant->thinker->set_aware(false);
        };
    };

    this->increment_count();
    this->spend_mana();

    return true;

    //
    //auto sneak?
};

SpawnShadowlingSpell::SpawnShadowlingSpell() : Spell()
{
    this->required_level = 8;
    this->name = "Spawn Shadowling";
    this->element = SpectreElement;
    this->mana_cost = 30;
    this->max_range = 2;
    this->aoe = 0;
    this->target_type = GroundTargetType;
};

bool SpawnShadowlingSpell::cast(Tile* targetted_tile)
{
    //get tiles within a given radius
    tile_vec_t* tiles = targetted_tile->getAdjacentTiles(this->aoe);

    //spawn creature at tile
    tile_vec_t::iterator it = tiles->begin();
    for (it; it != tiles->end(); it++)
    {
        Tile* tile = *it;
        if (!tile->is_occupied())
        {
            this->spawn(tile);
        };
    };

    this->spend_mana();
    this->increment_count();

    return true;
};

void SpawnShadowlingSpell::spawn(Tile* targetted_tile)
{
    Skeleton* creature = Game::spawn_creature_ally<Skeleton>(targetted_tile, "Shadowling", 1000, 'l', Game::current_map);
    creature->img_path = get_data_path()+ "img/dark_elf8x8.png";
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

bool BribeSpell::cast(Tile* targetted_tile)
{
    //get tiles within a given radius
    tile_vec_t* tiles = targetted_tile->getAdjacentTiles(this->aoe);

    tile_vec_t::iterator it = tiles->begin();
    for (it; it != tiles->end(); it++)
    {
        Tile* tile = *it;
        if (tile->is_occupied() && tile->occupant != this->master && tile->occupant->thinker != NULL)
        {
            if (! this->check_resistances(tile->occupant)) { continue; };
            tile->occupant->thinker->is_ally = true;
            this->spend_mana();
            this->increment_count();
        };
    };

    return false;
};

/* bloodmage */

VenipunctureSpell::VenipunctureSpell()
{
    this->required_level = 2;
    this->name = "Venipuncture";
    this->element = LifeElement;
    this->mana_cost = 50;

    this->buff_damage = true;
    this->attr_effect->damage->normal = 15;
    this->attr_effect->duration = 50;

    this->max_range = 1;
    this->target_type = TargettedTargetType;

    this->mana_type = BloodManaType;
    this->mana_percentage = true;
};

BonewallSpell::BonewallSpell()
{
    this->required_level = 4;
    this->name = "Bonewall";
    this->element = DeathElement;
    this->mana_cost = 5;

    // this->attr_effect->damage->normal = 15;
    // this->attr_effect->duration = 50;

    this->max_range = 6;
    this->target_type = GroundTargetType;

    this->mana_type = BloodManaType;
    this->mana_percentage = false;
};

bool BonewallSpell::cast(Tile* targetted_tile)
{
    if (targetted_tile->is_occupied())
    {
        return false;
    };

    targetted_tile->updateTileType(WallTileTypeType);

    this->increment_count();

    this->spend_mana();

    return true;
};

LimbBreakerSpell::LimbBreakerSpell()
{
    this->name = "Limb Breaker";
    this->element = DeathElement;
    this->attr_effect->speed_current_val = 500;
    this->attr_effect->speed_max_val = 500;

    this->max_range = 7;
    this->target_type = TargettedTargetType;

    this->mana_cost = 20;
    this->mana_type = BloodManaType;

    this->attr_effect->duration = 50;

    this->required_level = 6;
};

TormentorSpell::TormentorSpell()
{
    this->required_level = 8;
    this->name = "Tormentor";
    this->element = DeathElement;
    this->mana_cost = 90;

    // this->attr_effect->damage->normal = 15;
    // this->attr_effect->duration = 50;

    this->max_range = 2;
    this->target_type = GroundTargetType;

    this->mana_type = BloodManaType;
    this->mana_percentage = true;
};

bool TormentorSpell::cast(Tile* targetted_tile)
{
    if (targetted_tile->is_occupied())
    {
        return false;
    };

    CrazedCook* poor_soul = Game::spawn_creature_ally<CrazedCook>(targetted_tile, "Poor Soul", 1000, 'p', Game::current_map);
    this->master->soullink_to(poor_soul);

    this->spend_mana();
    this->increment_count();

    return true;
};

/* misc */

// SoulLinkSpell::SoulLinkSpell() : Spell()
// {
//     this->required_level = 8;
//     this->name = "Soullink";
//     this->element = SpectreElement;
//     this->mana_cost = 5;
//     this->max_range = 6;
//     this->target_type = GroundTargetType;
// };

// bool SoulLinkSpell::cast(Tile* targetted_tile)
// {
//     if (targetted_tile->is_occupied()) { return false; };
// 
//     //cast spell, apply attrs etc
//     if (targetted_tile->is_walkable())
//     {
//         this->master->put_person(targetted_tile, targetted_tile->tile_x, targetted_tile->tile_y);
//         if (this->master == Game::player)
//         {
//             Game::stats->spells_cast++;
//         };
//         this->spend_mana();
//         this->cast_count++;
// 
//         return true;
//     }
//     else
//     {
//         return false;
//         printf("Is not walkable\n");
//     }
// };

TeleportSelfSpell::TeleportSelfSpell() : Spell()
{
    this->required_level = 8;
    this->name = "Translocation";
    this->element = SpectreElement;
    this->mana_cost = 25;
    this->max_range = 6;
    this->target_type = GroundTargetType;
};

bool TeleportSelfSpell::cast(Tile* targetted_tile)
{
    if (targetted_tile->is_occupied()) { return false; };

    //cast spell, apply attrs etc
    if (targetted_tile->is_walkable())
    {
        this->master->put_person(targetted_tile, targetted_tile->tile_x, targetted_tile->tile_y);
        if (this->master == Game::player)
        {
            Game::stats->spells_cast++;
        };

        this->increment_count();
        return true;
    }
    else
    {
        return false;
        printf("Is not walkable\n");
    }
};

FireBallSpell::FireBallSpell() : Spell()
{
    this->required_level = 1;
    this->name = "Fireball";
    this->description = "Hurls a ball of fire, burns several enemies at a time.";
    this->element = FireElement;
    this->attr_effect->damage->fire = 10;
    this->mana_cost = 10;
    this->max_range = 10;
    this->aoe = 1;
    this->target_type = GroundTargetType;

    // this->mana_percentage = true;
    // this->mana_type = BloodManaType;
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

bool LaunchOtherSpell::cast(Tile* targetted_tile)
{
    //make sure someone is there
    if (!targetted_tile->is_occupied()) { return false; };

    //cast spell, apply attrs etc

    //if (targetted_tile->is_walkable())
    if (this->check_resistances(targetted_tile->occupant))
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
        targetted_tile->occupant->put_person(new_tile, new_tile->tile_x, new_tile->tile_y);
        if (this->master == Game::player)
        {
            Game::stats->spells_cast++;
        };
        this->spend_mana();
        this->increment_count();

        return true;
        // Spell::cast(targetted_tile);
    }
    else
    {
        return false;
    }

};

IlluminationSpell::IlluminationSpell() : Spell()
{
    this->name = "Illumination";
    this->target_type = GroundTargetType;
    this->max_range = 20;
};

bool IlluminationSpell::cast(Tile* targetted_tile)
{
    targetted_tile->setKnown(true);
    tile_vec_t* adjacent_tiles = targetted_tile->getAdjacentTiles(3);
    for (tile_vec_t_it it = adjacent_tiles->begin(); it != adjacent_tiles->end(); it++)
    {
        Tile* tile = *it;
        tile->setKnown(true);
    };
    delete adjacent_tiles;

    return true;
};
