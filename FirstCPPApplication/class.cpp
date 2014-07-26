#include "stdafx.h"
#include "class.h"
#include "actors\actor.h"
#include "attribute.h"
#include "attribute_container.h"
#include "ui.h"
#include "messages.h"
#include "spells.h"
#include "attr_effect.h"
#include "item.h"
#include <map>
#include "custom_key.h"

void IClass::apply_starting_stats()
{

        // int starting_health;
        // int starting_mana;
        // int starting_armor;
        // int starting_damage;

};

void IClass::LevelUpHealth(double change)
{
    this->master->attrs->health->current_val+=change;
    this->master->attrs->health->max_val+=change;
};
void IClass::LevelUpMana(double change)
{
    this->master->attrs->mana->current_val+=change;
    this->master->attrs->mana->max_val+=change;
};
void IClass::LevelUpArmor(double change)
{
    this->master->attrs->armor->current_val+=change;
    this->master->attrs->armor->max_val+=change;
};
void IClass::LevelUpDamage(double change)
{
    this->master->attrs->damage->current_val+=change;
    this->master->attrs->damage->max_val+=change;
};

void IClass::LevelUpSkills(int levels)
{
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, this->levelup_message);
    std::string msg = ss.str();

	auto new_spell_pair = this->spell_map->find(this->master->level);

    if (new_spell_pair != this->spell_map->end())
    {
        Spell* new_spell = (*new_spell_pair).second;
        new Message(Ui::msg_handler_main, MOOD_MSG, msg);
        new_spell->master = this->master;
        this->master->spells->push_back(new_spell);

        CustomKey* ck = Game::get_free_custom_key();
        if (ck != NULL)
        {
            ck->assign_spell(new_spell);
        };
    };

};

FighterClass::FighterClass()
{
    this->type = FighterClassType;
    this->name = "Fighter";
    this->description = "Basic Class, all around decent guy.";
    this->levelup_message = "You've learned a new technique";
    this->fg_color = TCODColor::celadon;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new WaterBombSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new AutoChemHPSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new PoisonCoughSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerSanctuarySpell()));
};

void FighterClass::LevelUpStats(int levels)
{
    int i = 0;
    while (i < levels)
    {
        this->LevelUpHealth(2);
        this->LevelUpMana(2);
        this->LevelUpArmor(0.25);
        this->LevelUpDamage(0.25);
        i++;
    };
};


StalkerClass::StalkerClass()
{
    this->type = StalkerClassType;
    this->name = "Stalker";
    this->description = "Stealth Class, sticks to the shadows.";
    this->levelup_message = "You've mastered a new shade!";
    this->fg_color = TCODColor::darkerSepia;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new ShadowRunSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new CastShadowSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new BribeSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new SpawnShadowlingSpell()));
};

void StalkerClass::LevelUpStats(int levels)
{
    int i = 0;
    while (i < levels)
    {
        this->LevelUpHealth(2);
        this->LevelUpMana(1.5);
        this->LevelUpArmor(0.25);
        this->LevelUpDamage(1);
        i++;
    };
};

MageClass::MageClass()
{
    this->type = MageClassType;
    this->name = "Mage";
    this->description = "Magic Class, feels more spiritual.";
    this->levelup_message = "You've mastered a new spell!";
    this->fg_color = TCODColor::desaturatedBlue;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new IceBoltSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new InnerHealingSpiritSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsTouchSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerFountainSpell()));
};

void MageClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(2);
    this->LevelUpMana(4);
    this->LevelUpArmor(0.1);
    this->LevelUpDamage(0.1);
};


NecromancerClass::NecromancerClass()
{
    this->type = NecromancerClassType;
    this->name = "Necromancer";
    this->description = "Undead Class, has a thing for the dead and buried.";
    this->levelup_message = "You've learned a new sorcery!";
    this->fg_color = TCODColor::darkGrey;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new CorpseBlastSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new SiphonSpiritSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new RaiseDeadSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerFireSpell()));

};

void NecromancerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(2);
    this->LevelUpMana(4);
    this->LevelUpArmor(0.1);
    this->LevelUpDamage(0.1);
};


BrawlerClass::BrawlerClass()
{
    this->type = BrawlerClassType;
    this->name = "Brawler";
    this->description = "Beefy Class, a bit of a meathead.";
    this->levelup_message = "You've learned a new prayer!";
    this->fg_color = TCODColor::lightChartreuse;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new IceFistSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new SacredKickSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsHandSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerStrengthSpell()));
};

void BrawlerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(4);
    this->LevelUpMana(1);
    this->LevelUpArmor(0.7);
    this->LevelUpDamage(.9);
};


BloodMageClass::BloodMageClass()
{
    this->type = BloodMageClassType;
    this->name = "BloodMage";
    this->description = "Bloody Class, more of a sicko, than anything else.";
    this->levelup_message = "You've learned a new ritual!";
    this->fg_color = TCODColor::lighterRed;

    this->spell_map = new std::map<int, Spell*>();
	this->spell_map->insert(std::make_pair<int, Spell*>(2, new IceFistSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(4, new SacredKickSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsHandSpell()));
	this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerStrengthSpell()));
};

void BloodMageClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(8);
    this->LevelUpMana(0.1);
    this->LevelUpArmor(0.4);
    this->LevelUpDamage(.9);
};

