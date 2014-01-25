#include "stdafx.h"

#include <string>

#include "attribute_container.h"
#include <actors/actor.h>
#include "attribute.h"
#include "utils.h"


AttributeContainer::AttributeContainer()
{
    this->owner = NULL;

    this->health = new HealthAttribute;
    this->mana = new ManaAttribute;
    this->armor = new ArmorAttribute;
    this->damage = new DamageAttribute;
};

void AttributeContainer::Update()
{
    this->health->Update();
    this->mana->Update();
    this->armor->Update();
    this->damage->Update();
};

void AttributeContainer::RegenerateAll()
{
    this->RegenerateHealth();
    this->RegenerateMana();
    this->RegenerateArmor();
    this->RegenerateDamage();
};

void AttributeContainer::RegenerateHealth()
{
    this->health->Regenerate();

};

void AttributeContainer::RegenerateMana()
{
    this->mana->Regenerate();

};

void AttributeContainer::RegenerateArmor()
{
    this->armor->Regenerate();

};

void AttributeContainer::RegenerateDamage()
{
    this->damage->Regenerate();

};

std::vector<std::string> AttributeContainer::PrettyVector()
{
    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back("HCV: "+std::to_string((long double)this->health->current_val));
    string_vec.push_back("HMV: "+std::to_string((long double)this->health->max_val));
    string_vec.push_back("HRR: "+std::to_string((long double)this->health->regen_rate));
    string_vec.push_back("HRI: "+std::to_string((long double)this->health->regen_interval));

    string_vec.push_back("MCV: "+std::to_string((long double)this->mana->current_val));
    string_vec.push_back("MMV: "+std::to_string((long double)this->mana->max_val));
    string_vec.push_back("MRR: "+std::to_string((long double)this->mana->regen_rate));
    string_vec.push_back("MRI: "+std::to_string((long double)this->mana->regen_interval));

    string_vec.push_back("ACV: "+std::to_string((long double)this->armor->current_val));
    string_vec.push_back("AMV: "+std::to_string((long double)this->armor->max_val));
    string_vec.push_back("ARR: "+std::to_string((long double)this->armor->regen_rate));
    string_vec.push_back("ARI: "+std::to_string((long double)this->armor->regen_interval));

    string_vec.push_back("DCV: "+std::to_string((long double)this->damage->current_val));
    string_vec.push_back("DMV: "+std::to_string((long double)this->damage->max_val));
    string_vec.push_back("DRR: "+std::to_string((long double)this->damage->regen_rate));
    string_vec.push_back("DRI: "+std::to_string((long double)this->damage->regen_interval));

    return string_vec;
};

std::string AttributeContainer::PrettyPrint()
{
    return StringJoin(this->PrettyVector(), '\n', false);
};
