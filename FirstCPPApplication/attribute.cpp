#include "stdafx.h"
#include "attribute.h"

Attribute::Attribute()
{
    this->tick = 0;
    this->is_ready = false;
    this->current_val = 999;
    this->max_val = 999;
    this->regen_rate = 1;
    this->regen_interval = 1;

};

Attribute::Attribute(Attribute& other)
{
    this->tick = other.tick;
    this->is_ready = other.is_ready;
    this->current_val = other.current_val;
    this->max_val = other.max_val;
    this->regen_rate = other.regen_rate;
    this->regen_interval = other.regen_interval;

};

float Attribute::GetValPercentage()
{
    float percent = (float)(this->current_val)/ ((float)this->max_val) * 100;
    return percent;

};

void Attribute::Update()
{
    this->Tick();

    if (this->CheckIsReady())
    {
        this->Regenerate();
        // this->AddToCurrentVal(this->regen_rate);
    };

};

void Attribute::SetVals(int both)
{
    this->SetVals(both, both);
};

void Attribute::SetVals(int current_val, int max_val)
{
    this->current_val = current_val;
    this->max_val = max_val;
};

void Attribute::Tick()
{

    //if there's no regen rate, freeze and reset regen
    if (this->regen_rate == 0)
    {
        this->is_ready = false;
        this->tick = 1;
        return;
    }

    if (this->tick < this->regen_interval)
    {
        this->tick += 1;
        this->is_ready = false;
    }
    else
    {
        this->tick = 1;
        this->is_ready = true;
    };

};

void Attribute::AddToCurrentVal(int difference)
{
    if (difference != 0)
    {
        this->current_val += difference;
        if (this->current_val > this->max_val)
        {
            this->current_val = this->max_val;
        };
    };

};

void Attribute::AddToMaxVal(int difference)
{
    if (difference != 0)
    {
        this->max_val += difference;
    };

};

void Attribute::AddToRegenRate(int difference)
{
    if (difference != 0)
    {
        this->regen_rate += difference;
    };

};

void Attribute::AddToRegenInterval(int difference)
{
    if (difference != 0)
    {
        this->regen_interval += difference;
    };

};

void Attribute::RemoveFromCurrentVal(int difference)
{
    if (difference != 0)
    {
        this->current_val -= difference;
    };

};

void Attribute::RemoveFromMaxVal(int difference)
{
    if (difference != 0)
    {
        this->max_val -= difference;
    };

};

void Attribute::RemoveFromRegenRate(int difference)
{
    if (difference != 0)
    {
        this->regen_rate -= difference;
    };

};

void Attribute::RemoveFromRegenInterval(int difference)
{
    if (difference != 0)
    {
        this->regen_interval -= difference;
    };

};

// bool Attribute::CanRegenerate()
// {
//     //TODO add check to see if has enough intervals has been passed
//     return true;
// };

void Attribute::Regenerate()
{
    if (this->CheckIsReady())
    {
        this->current_val += this->regen_rate;
        if (this->current_val > this->max_val)
        {
            this->current_val = this->max_val;
        }
    };

};

bool Attribute::CheckIsReady()
{
    return this->is_ready;
};

//void HealthAttribute::Regenerate()
//{
//	Attribute::Regenerate();
//
//};

HealthAttribute::HealthAttribute() : Attribute()
{
    this->max_val = 100;
    this->current_val = this->max_val;
    this->regen_interval = 3;

};

ManaAttribute::ManaAttribute() : Attribute()
{
    this->max_val = 25;
    this->current_val = this->max_val;

    this->regen_rate = 1;
    this->regen_interval = 3;

};

ArmorAttribute::ArmorAttribute() : Attribute()
{
    this->max_val = 99999;
    this->current_val = 1;

    this->regen_rate = 0;
    // this->max_val = 1;
    // this->current_val = this->max_val;

    // this->regen_rate = 0;
};

DamageAttribute::DamageAttribute() : Attribute()
{
    this->max_val = 99999;
    this->current_val = 10;

    this->regen_rate = 0;

};
HungerAttribute::HungerAttribute() : Attribute()
{
    this->max_val = 999999;
    this->current_val = 0;

    this->regen_rate = 2;

};
