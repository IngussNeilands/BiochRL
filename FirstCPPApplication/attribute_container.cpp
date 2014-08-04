#include "stdafx.h"


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
    this->hunger = new HungerAttribute;
    this->speed = new SpeedAttribute;
};

AttributeContainer::AttributeContainer(AttributeContainer& other)
{
    this->owner = other.owner;

    this->health = new HealthAttribute(*other.health);
    this->mana = new ManaAttribute(*other.mana);
    this->armor = new ArmorAttribute(*other.armor);
    this->damage = new DamageAttribute(*other.damage);
    this->hunger = new HungerAttribute(*other.hunger);
    this->speed = new SpeedAttribute(*other.speed);
};

void AttributeContainer::Update()
{
    this->health->Update();
    this->mana->Update();
    this->armor->Update();
    this->damage->Update();
    this->hunger->Update();
    this->speed->Update();
};

void AttributeContainer::RegenerateAll()
{
    this->RegenerateHealth();
    this->RegenerateMana();
    this->RegenerateArmor();
    this->RegenerateDamage();
    this->RegenerateSpeed();
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

void AttributeContainer::RegenerateSpeed()
{
    this->speed->Regenerate();
};

std::vector<std::string> AttributeContainer::PrettyVectorColored()
{
    std::string result = "";
    std::vector<std::string> string_vec;

    TCODColor health_color = HealthAttribute::attribute_color;
    TCODColor mana_color = ManaAttribute::attribute_color;
    TCODColor armor_color = ArmorAttribute::attribute_color;
    TCODColor damage_color = DamageAttribute::attribute_color;
    TCODColor speed_color = SpeedAttribute::attribute_color;

    string_vec.push_back(buffer_color("HCV", this->health->current_val, health_color));
    string_vec.push_back(buffer_color("HMV", this->health->max_val, health_color));
    string_vec.push_back(buffer_color("HRR", this->health->regen_rate, health_color));
    string_vec.push_back(buffer_color("HRI", this->health->regen_interval, health_color));

    string_vec.push_back(buffer_color("MCV", this->mana->current_val, mana_color));
    string_vec.push_back(buffer_color("MMV", this->mana->max_val, mana_color));
    string_vec.push_back(buffer_color("MRR", this->mana->regen_rate, mana_color));
    string_vec.push_back(buffer_color("MRI", this->mana->regen_interval, mana_color));

    string_vec.push_back(buffer_color("ACV", this->armor->current_val, armor_color));
    string_vec.push_back(buffer_color("AMV", this->armor->max_val, armor_color));
    string_vec.push_back(buffer_color("ARR", this->armor->regen_rate, armor_color));
    string_vec.push_back(buffer_color("ARI", this->armor->regen_interval, armor_color));

    string_vec.push_back(buffer_color("DCV", this->damage->current_val, damage_color));
    string_vec.push_back(buffer_color("DMV", this->damage->max_val, damage_color));
    string_vec.push_back(buffer_color("DRR", this->damage->regen_rate, damage_color));
    string_vec.push_back(buffer_color("DRI", this->damage->regen_interval, damage_color));

    string_vec.push_back(buffer_color("SCV", this->speed->current_val, speed_color));
    string_vec.push_back(buffer_color("SMV", this->speed->max_val, speed_color));
    string_vec.push_back(buffer_color("SRR", this->speed->regen_rate, speed_color));
    string_vec.push_back(buffer_color("SRI", this->speed->regen_interval, speed_color));

    return string_vec;
};

template<typename AttrClass>
std::string format_verbose(AttrClass* attr)
{
    std::stringstream ss;
    // ss << attr->current_val << "/" << attr->max_val;
    ss << attr->max_val << "pts";
    std::string temp = ss.str();
    ss.str("");
    ss << colfg(AttrClass::attribute_color, temp) << " regen " << attr->regen_rate << "pts/" << attr->regen_interval << "turns";
    return ss.str();
};

std::vector<std::string> AttributeContainer::VerboseVector()
{
    std::string result = "";
    std::vector<std::string> string_vec;

    TCODColor health_color = HealthAttribute::attribute_color;
    TCODColor mana_color = ManaAttribute::attribute_color;
    TCODColor armor_color = ArmorAttribute::attribute_color;
    TCODColor damage_color = DamageAttribute::attribute_color;
    TCODColor speed_color = SpeedAttribute::attribute_color;

    string_vec.push_back(colfg(health_color, "HEALTH"));
    string_vec.push_back(format_verbose<HealthAttribute>(this->health));

    string_vec.push_back(colfg(mana_color, "MANA"));
    string_vec.push_back(format_verbose<ManaAttribute>(this->mana));

    string_vec.push_back(colfg(armor_color, "ARMOR"));
    string_vec.push_back(format_verbose<ArmorAttribute>(this->armor));

    string_vec.push_back(colfg(damage_color, "DAMAGE"));
    string_vec.push_back(format_verbose<DamageAttribute>(this->damage));

    string_vec.push_back(colfg(speed_color, "SPEED"));
    string_vec.push_back(format_verbose<SpeedAttribute>(this->speed));

    return string_vec;
};

std::vector<std::string> AttributeContainer::PrettyVector()
{
    std::string result = "";
    std::vector<std::string> string_vec;

    string_vec.push_back("HCV: "+std::to_string((long double)(int)this->health->current_val));
    string_vec.push_back("HMV: "+std::to_string((long double)(int)this->health->max_val));
    string_vec.push_back("HRR: "+std::to_string((long double)(int)this->health->regen_rate));
    string_vec.push_back("HRI: "+std::to_string((long double)(int)this->health->regen_interval));

    string_vec.push_back("MCV: "+std::to_string((long double)(int)this->mana->current_val));
    string_vec.push_back("MMV: "+std::to_string((long double)(int)this->mana->max_val));
    string_vec.push_back("MRR: "+std::to_string((long double)(int)this->mana->regen_rate));
    string_vec.push_back("MRI: "+std::to_string((long double)(int)this->mana->regen_interval));

    string_vec.push_back("ACV: "+std::to_string((long double)(int)this->armor->current_val));
    string_vec.push_back("AMV: "+std::to_string((long double)(int)this->armor->max_val));
    string_vec.push_back("ARR: "+std::to_string((long double)(int)this->armor->regen_rate));
    string_vec.push_back("ARI: "+std::to_string((long double)(int)this->armor->regen_interval));

    string_vec.push_back("DCV: "+std::to_string((long double)(int)this->damage->current_val));
    string_vec.push_back("DMV: "+std::to_string((long double)(int)this->damage->max_val));
    string_vec.push_back("DRR: "+std::to_string((long double)(int)this->damage->regen_rate));
    string_vec.push_back("DRI: "+std::to_string((long double)(int)this->damage->regen_interval));

    string_vec.push_back("SCV: "+std::to_string((long double)(int)this->speed->current_val));
    string_vec.push_back("SMV: "+std::to_string((long double)(int)this->speed->max_val));
    string_vec.push_back("SRR: "+std::to_string((long double)(int)this->speed->regen_rate));
    string_vec.push_back("SRI: "+std::to_string((long double)(int)this->speed->regen_interval));

    return string_vec;
};

std::string AttributeContainer::PrettyPrint()
{
    return StringJoin(this->PrettyVector(), '\n', false);
};
