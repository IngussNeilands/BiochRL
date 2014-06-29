#pragma once
#ifndef SPELLS_H
#define SPELLS_H

#include <string>
#include <unordered_map>
#include "game.h"
#include "actors\actor.h"
#include "enums\elements_t.h"

class AttributeContainer;
class Attribute;
class AttrEffect;
class Actor;
class Damage;
class Armor;
class CustomKey;

enum TargetTypes {
    TargettedTargetType = 0,
    GroundTargetType = 1
};

class Spell 
{
    public:

        static const std::unordered_map<elements_t, TCODColor>* spell_color;

        std::string name;
        int mana_cost;

        elements_t element;

        int required_level;

        int turn_cooldown;
        int max_range;
        int min_range;
        int min_char_level;
        int cast_count;

        CustomKey* custom_key;

        Actor* master;

        int aoe;
        TargetTypes target_type;

        AttributeContainer* attrs;
        AttrEffect* attr_effect;

        Spell();
        TCODColor get_spell_color();

        bool is_valid_target(Tile* targetted_tile);
        bool is_in_range(int distance);
        bool has_enough_mana();

        virtual void cast(Tile* targetted_tile);
        virtual void apply_attr_effects(Actor* target);
        std::vector<Actor*> targets_around_tile(Tile* target_tile);

};

//fighter
class WaterBombSpell : public Spell {  public: WaterBombSpell();  };
class AutoChemHPSpell : public Spell {  public: AutoChemHPSpell();  };
class PoisonCoughSpell : public Spell {  public: PoisonCoughSpell();  };
class InnerSanctuarySpell : public Spell {  public: InnerSanctuarySpell();  };

//mage
class IceBoltSpell : public Spell {  public: IceBoltSpell(); };
class InnerHealingSpiritSpell : public Spell {  public: InnerHealingSpiritSpell(); };
class DeathsTouchSpell : public Spell {  public: DeathsTouchSpell(); };
class InnerFountainSpell : public Spell {  public: InnerFountainSpell(); };

//necro
class CorpseBlastSpell : public Spell 
{
    public:
        CorpseBlastSpell(); 
        void cast(Tile* targetted_tile);
};

class SiphonSpiritSpell : public Spell 
{
    public: 
        SiphonSpiritSpell(); 
        void apply_attr_effects(Actor* target);

};
class RaiseDeadSpell : public Spell 
{
    public: 
        RaiseDeadSpell();
        void cast(Tile* targetted_tile);
        void raise_dead(Tile* targetted_tile);
};
class InnerFireSpell : public Spell {  public: InnerFireSpell(); };

//brawler
class IceFistSpell : public Spell {  public: IceFistSpell(); };
class SacredKickSpell : public Spell {  public: SacredKickSpell(); };
class DeathsHandSpell : public Spell {  public: DeathsHandSpell(); };
class InnerStrengthSpell : public Spell {  public: InnerStrengthSpell(); };


//misc
class FireBallSpell : public Spell {  public: FireBallSpell(); };

class TeleportSelfSpell : public Spell
{
    public:
        TeleportSelfSpell();
        void cast(Tile* targetted_tile);
};

class CastShadowSpell : public Spell
{
    public:
        CastShadowSpell();
        void cast(Tile* targetted_tile);
};

class ShadowRunSpell : public TeleportSelfSpell
{
    public:
        ShadowRunSpell();
};

class LaunchOtherSpell : public Spell
{
    public:
        int force;
        LaunchOtherSpell();
        void cast(Tile* targetted_tile);

};

#endif
