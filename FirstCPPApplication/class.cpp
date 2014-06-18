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
    this->type = 1;
    this->name = "Fighter";
    this->description = "Basic Class, all around decent guy.";
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
    Spell* new_spell;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new technique!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            // new Message(Ui::msg_handler_main, MOOD_MSG, "%c%c%c%cYou've learned a new spell!%c", TCOD_COLCTRL_FORE_RGB, (int)TCODColor::blue.r, (int)TCODColor::blue.g, (int)TCODColor::blue.b, TCOD_COLCTRL_STOP);
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new WaterBombSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 4:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new AutoChemHPSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 6:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new PoisonCoughSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 8:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new InnerSanctuarySpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

    };

};

MageClass::MageClass()
{
    this->type = 2;
    this->name = "Mage";
    this->description = "Magic Class, feels more spiritual.";
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
    Spell* new_spell;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new spell!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            // new Message(Ui::msg_handler_main, MOOD_MSG, "%c%c%c%cYou've learned a new spell!%c", TCOD_COLCTRL_FORE_RGB, (int)TCODColor::blue.r, (int)TCODColor::blue.g, (int)TCODColor::blue.b, TCOD_COLCTRL_STOP);
            new_spell = new IceBoltSpell();
            new_spell->master = this->master;
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            this->master->spells->push_back(new_spell);
            break;

        case 4:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new InnerHealingSpiritSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 6:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new DeathsTouchSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 8:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new FireBallSpell;
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

    };

};

NecromancerClass::NecromancerClass()
{
    this->type = 4;
    this->name = "Necromancer";
    this->description = "Undead Class, has a thing for the dead and buried.";
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
    Spell* new_spell;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new sorcery!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            // new Message(Ui::msg_handler_main, MOOD_MSG, "%c%c%c%cYou've learned a new spell!%c", TCOD_COLCTRL_FORE_RGB, (int)TCODColor::blue.r, (int)TCODColor::blue.g, (int)TCODColor::blue.b, TCOD_COLCTRL_STOP);
            new_spell = new CorpseBlastSpell();
            new_spell->master = this->master;
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            this->master->spells->push_back(new_spell);
            break;

        case 4:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new SiphonSpiritSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 6:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new RaiseDeadSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 8:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new InnerFireSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

    };

};

BrawlerClass::BrawlerClass()
{
    this->type = 3;
    this->name = "Brawler";
    this->description = "Beefy Class, a bit of a meathead.";
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
    Spell* new_spell;
    std::stringstream ss;
    ss << colfg(TCODColor::lighterAzure, "You've learned a new prayer!");
    std::string msg = ss.str();
    switch (this->master->level)
    {
        case 2:
            // new Message(Ui::msg_handler_main, MOOD_MSG, "%c%c%c%cYou've learned a new spell!%c", TCOD_COLCTRL_FORE_RGB, (int)TCODColor::blue.r, (int)TCODColor::blue.g, (int)TCODColor::blue.b, TCOD_COLCTRL_STOP);
            new_spell = new IceFistSpell();
            new_spell->master = this->master;
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            this->master->spells->push_back(new_spell);
            break;

        case 4:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new SacredKickSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 6:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new DeathsHandSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

        case 8:
            new Message(Ui::msg_handler_main, MOOD_MSG, msg);
            new_spell = new InnerStrengthSpell();
            new_spell->master = this->master;
            this->master->spells->push_back(new_spell);
            break;

    };

};
