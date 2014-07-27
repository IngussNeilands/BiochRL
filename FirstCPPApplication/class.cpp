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

    if (this->master != NULL)
    {
        this->master->attrs = new AttributeContainer(*this->starting_attrs);
        std::cout << "Updated attrs to " << this->name << std::endl;
    };

}; 

IClass::IClass()
{ 
    this->type = NoClassType;

    this->starting_attrs = new AttributeContainer();
	this->starting_attrs->owner = NULL;
    this->starting_attrs->health->SetVals(30);
    this->starting_attrs->mana->SetVals(30);
    this->starting_attrs->damage->SetVals(6);
    this->starting_attrs->armor->SetVals(1);

    this->spell_map = new std::map<int, Spell*>();

    this->health_on_lvl = 1;
    this->mana_on_lvl = 1;
    this->armor_on_lvl = 1;
    this->damage_on_lvl = 1;
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

void IClass::LevelUpHealth()
{
    this->LevelUpHealth(this->health_on_lvl);
};
void IClass::LevelUpMana()
{
    this->LevelUpMana(this->mana_on_lvl);
};
void IClass::LevelUpArmor()
{
    this->LevelUpArmor(this->armor_on_lvl);
};
void IClass::LevelUpDamage()
{
    this->LevelUpDamage(this->damage_on_lvl);
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

FighterClass::FighterClass() : IClass()
{
    this->type = FighterClassType;
    this->name = "Fighter";
    this->description = "Basic Class, all around decent guy.";
    this->levelup_message = "You've learned a new technique";
    this->fg_color = TCODColor::celadon;

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new WaterBombSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new AutoChemHPSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new PoisonCoughSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerSanctuarySpell()));
    this->health_on_lvl = 2;
    this->mana_on_lvl = 2;
    this->armor_on_lvl = 0.25;
    this->damage_on_lvl = 0.25;
};

void FighterClass::LevelUpStats(int levels)
{
    int i = 0;
    while (i < levels)
    {
        this->LevelUpHealth();
        this->LevelUpMana();
        this->LevelUpArmor();
        this->LevelUpDamage();
        i++;
    };
};


StalkerClass::StalkerClass() : IClass()
{
    this->type = StalkerClassType;
    this->name = "Stalker";
    this->description = "Stealth Class, sticks to the shadows.";
    this->levelup_message = "You've mastered a new shade!";
    this->fg_color = TCODColor::darkerSepia;

    this->starting_attrs->health->SetVals(25);

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new ShadowRunSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new CastShadowSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new BribeSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new SpawnShadowlingSpell()));

    this->health_on_lvl = 2;
    this->mana_on_lvl = 1.5;
    this->armor_on_lvl = 0.25;
    this->damage_on_lvl = 1;
};

void StalkerClass::LevelUpStats(int levels)
{
    int i = 0;
    while (i < levels)
    {
        this->LevelUpHealth();
        this->LevelUpMana();
        this->LevelUpArmor();
        this->LevelUpDamage();
        i++;
    };
};

MageClass::MageClass() : IClass()
{
    this->type = MageClassType;
    this->name = "Mage";
    this->description = "Magic Class, feels more spiritual.";
    this->levelup_message = "You've mastered a new spell!";
    this->fg_color = TCODColor::desaturatedBlue;

    this->starting_attrs->mana->SetVals(50);
    this->starting_attrs->mana->regen_rate = 2;

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new IceBoltSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new InnerHealingSpiritSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsTouchSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerFountainSpell()));

    this->health_on_lvl = 2;
    this->mana_on_lvl = 4;
    this->armor_on_lvl = 0.1;
    this->damage_on_lvl = 0.1;
};

void MageClass::LevelUpStats(int levels)
{
    this->LevelUpHealth();
    this->LevelUpMana();
    this->LevelUpArmor();
    this->LevelUpDamage();
};


NecromancerClass::NecromancerClass() : IClass()
{
    this->type = NecromancerClassType;
    this->name = "Necromancer";
    this->description = "Undead Class, has a thing for the dead and buried.";
    this->levelup_message = "You've learned a new sorcery!";
    this->fg_color = TCODColor::darkGrey;

    this->starting_attrs->mana->SetVals(35);

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new CorpseBlastSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new SiphonSpiritSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new RaiseDeadSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerFireSpell()));

    this->health_on_lvl = 2;
    this->mana_on_lvl = 4;
    this->armor_on_lvl = 0.1;
    this->damage_on_lvl = 0.1;
};

void NecromancerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth();
    this->LevelUpMana();
    this->LevelUpArmor();
    this->LevelUpDamage();
};


BrawlerClass::BrawlerClass() : IClass()
{
    this->type = BrawlerClassType;
    this->name = "Brawler";
    this->description = "Beefy Class, a bit of a meathead.";
    this->levelup_message = "You've learned a new prayer!";
    this->fg_color = TCODColor::lightChartreuse;

    this->starting_attrs->health->SetVals(45);
    this->starting_attrs->armor->SetVals(3);
    this->starting_attrs->damage->SetVals(8);

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new IceFistSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new SacredKickSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsHandSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerStrengthSpell()));

    this->health_on_lvl = 4;
    this->mana_on_lvl = 1;
    this->armor_on_lvl = 0.7;
    this->damage_on_lvl = .9;
};

void BrawlerClass::LevelUpStats(int levels)
{
    this->LevelUpHealth();
    this->LevelUpMana();
    this->LevelUpArmor();
    this->LevelUpDamage();
};


BloodMageClass::BloodMageClass() : IClass()
{
    this->type = BloodMageClassType;
    this->name = "BloodMage";
    this->description = "Bloody Class, more of a sicko, than anything else.";
    this->levelup_message = "You've learned a new ritual!";
    this->fg_color = TCODColor::lighterRed;

    this->starting_attrs->health->SetVals(60);
    this->starting_attrs->health->regen_interval = 5;
    this->starting_attrs->health->regen_rate = 2;

    this->starting_attrs->mana->SetVals(15);
    this->starting_attrs->mana->regen_interval = 10;

    this->spell_map->insert(std::make_pair<int, Spell*>(2, new Venipuncture()));
    this->spell_map->insert(std::make_pair<int, Spell*>(4, new SacredKickSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(6, new DeathsHandSpell()));
    this->spell_map->insert(std::make_pair<int, Spell*>(8, new InnerStrengthSpell()));

    this->health_on_lvl = 8;
    this->mana_on_lvl = 0.1;
    this->armor_on_lvl = 0.4;
    this->damage_on_lvl = .9;
};

void BloodMageClass::LevelUpStats(int levels)
{
    this->LevelUpHealth();
    this->LevelUpMana();
    this->LevelUpArmor();
    this->LevelUpDamage();
};

