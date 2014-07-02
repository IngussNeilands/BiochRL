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
#include <color_utils.h>
#include <map>

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

void FighterClass::LevelUpSkills(int levels)
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
    };


};

StalkerClass::StalkerClass()
{
    this->type = StalkerClassType;
    this->name = "Stalker";
    this->description = "Stealth Class, sticks to the shadows.";
    this->fg_color = TCODColor::darkerSepia;
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

void StalkerClass::LevelUpSkills(int levels)
{
    Spell* new_spell = NULL;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new technique!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            new_spell = new ShadowRunSpell();
            break;

        case 4:
            new_spell = new CastShadowSpell();
            break;

        case 6:
            new_spell = new BribeSpell();
            break;

        case 8:
            new_spell = new SpawnShadowlingSpell();
            break;

    };

    if (new_spell != NULL)
    {
        new Message(Ui::msg_handler_main, MOOD_MSG, msg);
        new_spell->master = this->master;
        this->master->spells->push_back(new_spell);
    };

};

MageClass::MageClass()
{
    this->type = MageClassType;
    this->name = "Mage";
    this->description = "Magic Class, feels more spiritual.";
    this->fg_color = TCODColor::desaturatedBlue;
};

void MageClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(2);
    this->LevelUpMana(4);
    this->LevelUpArmor(0.1);
    this->LevelUpDamage(0.1);
};

void MageClass::LevelUpSkills(int levels)
{
    Spell* new_spell = NULL;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new spell!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            new_spell = new IceBoltSpell();
            break;

        case 4:
            new_spell = new InnerHealingSpiritSpell();
            break;

        case 6:
            new_spell = new DeathsTouchSpell();
            break;

        case 8:
            new_spell = new FireBallSpell;
            break;

    };

    if (new_spell != NULL)
    {
        new Message(Ui::msg_handler_main, MOOD_MSG, msg);
        new_spell->master = this->master;
        this->master->spells->push_back(new_spell);
    };

};

NecromancerClass::NecromancerClass()
{
    this->type = NecromancerClassType;
    this->name = "Necromancer";
    this->description = "Undead Class, has a thing for the dead and buried.";
    this->fg_color = TCODColor::darkGrey;

};

void NecromancerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(2);
    this->LevelUpMana(4);
    this->LevelUpArmor(0.1);
    this->LevelUpDamage(0.1);
};

void NecromancerClass::LevelUpSkills(int levels)
{
    Spell* new_spell = NULL;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new sorcery!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            new_spell = new CorpseBlastSpell();
            break;

        case 4:
            new_spell = new SiphonSpiritSpell();
            break;

        case 6:
            new_spell = new RaiseDeadSpell();
            break;

        case 8:
            new_spell = new InnerFireSpell();
            break;

    };

    if (new_spell != NULL)
    {
        new Message(Ui::msg_handler_main, MOOD_MSG, msg);
        new_spell->master = this->master;
        this->master->spells->push_back(new_spell);
    };


};

BrawlerClass::BrawlerClass()
{
    this->type = BrawlerClassType;
    this->name = "Brawler";
    this->description = "Beefy Class, a bit of a meathead.";
    this->fg_color = TCODColor::lightChartreuse;
};

void BrawlerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth(4);
    this->LevelUpMana(1);
    this->LevelUpArmor(0.7);
    this->LevelUpDamage(.9);
};

void BrawlerClass::LevelUpSkills(int levels)
{
    Spell* new_spell = NULL;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new prayer!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            // new Message(Ui::msg_handler_main, MOOD_MSG, "%c%c%c%cYou've learned a new spell!%c", TCOD_COLCTRL_FORE_RGB, (int)TCODColor::blue.r, (int)TCODColor::blue.g, (int)TCODColor::blue.b, TCOD_COLCTRL_STOP);
            new_spell = new IceFistSpell();
            break;

        case 4:
            new_spell = new SacredKickSpell();
            break;

        case 6:
            new_spell = new DeathsHandSpell();
            break;

        case 8:
            new_spell = new InnerStrengthSpell();
            break;

    };

    if (new_spell != NULL)
    {
        new Message(Ui::msg_handler_main, MOOD_MSG, msg);
        new_spell->master = this->master;
        this->master->spells->push_back(new_spell);
    };

};
