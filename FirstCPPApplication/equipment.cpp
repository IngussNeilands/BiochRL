#include "stdafx.h"
#include <assert.h>


#include "equipment.h"
#include "item.h"
#include <actors/actor.h>
#include "attr_effect.h"

Slot::Slot(slots_t type, Equipment* equipment)
{
    this->type = type;
    this->equipment = equipment;
    this->equipped_item = NULL;

    this->equipment->slots->push_back(this);

};

bool Slot::HasRoomFor(Item* item)
{
    return equipped_item == NULL;

};


void Slot::AddToSlot(Item* item)
{
    //if item slot isn't full
    if (this->HasRoomFor(item))
    {
        //add item to slot
        //std::cout << "added item to slot" << std::endl;
        this->equipped_item = item;
        // this->equipped_item->attr_effect->ApplyAllEffects(this->equipment->master);
        this->apply_attr_effect();
    }
    else
    {
        std::cout << "DID NOT HAVE ROOM to  item to slot" << std::endl;
    };

};

void Slot::RemoveFromSlot()
{
    this->remove_attr_effect();
    this->equipped_item = NULL;

};

Actor* Slot::get_master()
{
    if (this->equipment->master != NULL)
    {
        return this->equipment->master;
    }
    else
    {
        return NULL;
    };
};

void Slot::remove_attr_effect()
{
    //find equipment master
    Actor* master = this->get_master();
    //check if attributes
    if (master != NULL && master->has_attributes(), this->equipped_item != NULL)
    {
        //apply appropriate health mana damage armor changes

        this->equipped_item->attr_effect->RemoveHealthEffects(master);
        this->equipped_item->attr_effect->RemoveManaEffects(master);
        this->equipped_item->attr_effect->RemoveArmorEffects(master);
        //this->equipped_item->attr_effect->RemoveDamageEffects(master);
        this->equipped_item->attr_effect->unmark_applied_all(master);

    }

};

void Slot::apply_attr_effect()
{

    //find equipment master
    Actor* master = this->get_master();
    //check if attributes
    if (master != NULL && master->has_attributes(), this->equipped_item != NULL)
    {
        //apply appropriate health mana damage armor changes
        this->equipped_item->attr_effect->ApplyHealthEffects(master);
        this->equipped_item->attr_effect->ApplyManaEffects(master);
        this->equipped_item->attr_effect->ApplyArmorEffects(master);
        // this->equipped_item->attr_effect->ApplyDamageEffects(master);
    }

    //TODO TODO TODO
    //add equipment update to main loop

};

Item* Slot::GetEquippedItem()
{
    if (this->equipped_item != NULL)
    {
        return this->equipped_item;
    }
    return NULL;
};


bool Slot::CanFitInSlot(Item* item)
{
    return this->type==item->slot_type;

};

Equipment::Equipment()
{
    this->master = NULL;
    this->slots = new std::vector<Slot*>;

    this->head = new Slot(slots_t::Head, this);
    this->earrings = new Slot(slots_t::Ear, this);
    this->necklace = new Slot(slots_t::Neck, this);

    this->chest = new Slot(slots_t::Chest, this);
    this->left_shoulder = new Slot(slots_t::Shoulders, this);
    this->right_shoulder = new Slot(slots_t::Shoulders, this);

    this->left_bracer = new Slot(slots_t::Forearms, this);
    this->right_bracer = new Slot(slots_t::Forearms, this);

    this->left_glove = new Slot(slots_t::Hands, this);
    this->right_glove = new Slot(slots_t::Hands, this);

    this->left_ring_ring = new Slot(slots_t::Fingers, this);
    this->right_ring_ring = new Slot(slots_t::Fingers, this);
    this->left_ring_middle = new Slot(slots_t::Fingers, this);
    this->right_ring_middle = new Slot(slots_t::Fingers, this);
    this->left_ring_index = new Slot(slots_t::Fingers, this);
    this->right_ring_index = new Slot(slots_t::Fingers, this);

    this->main_weapon = new Slot(slots_t::MainHand, this);
    this->off_weapon = new Slot(slots_t::OffHand, this);

    this->left_thigh = new Slot(slots_t::Thighs, this);
    this->right_thigh = new Slot(slots_t::Thighs, this);
    this->left_shin = new Slot(slots_t::Shins, this);
    this->right_shin = new Slot(slots_t::Shins, this);
    this->left_foot = new Slot(slots_t::Feet, this);
    this->right_foot = new Slot(slots_t::Feet, this);


};

int Equipment::get_primary_range()
{
    int result = Equipment::default_primary_range;
    if (this->main_weapon->equipped_item != NULL)
    {
        result = this->main_weapon->equipped_item->range;
    }

    return result;
};

Slot* Equipment::get_slots_for_type(slots_t slot_type)
{

    assert(slot_type != NoSlot && "noslots have, by definition, no slot, what are you trying to do?");
    for (std::vector<Slot*>::iterator it = this->slots->begin(); it != this->slots->end(); it++)
    {
        if ( (*it)->type == slot_type)
        {
            return (*it);
        }
    }

    return NULL;

}

item_vec_t Equipment::get_items_equipped()
{
    item_vec_t items = item_vec_t(); 

    for (auto it = this->slots->begin(); it!=this->slots->end(); it++)
    {
        Slot* slot = *it;
        Item* item = slot->GetEquippedItem();
        if (item != NULL)
        {
            items.push_back(item);
        }
    };

    return items;
};

spell_vec_t Equipment::get_spells_attached()
{

    spell_vec_t spells = spell_vec_t();

    auto equipment = this->get_items_equipped();
    for (auto it = equipment.begin(); it!=equipment.end(); it++)
    {
        Item* item = *it;
        if (item->spell_effect != NULL)
        {
            spells.push_back(item->spell_effect);
        }
    }

    return spells;
};

void Equipment::equip_item(Item* item)
{

    // NOTE DOESNT apply item effects

    //get item slot type
    slots_t slot_type = item->slot_type;

    //check if slot is empty
    for (std::vector<Slot*>::iterator it = this->slots->begin(); it != this->slots->end(); ++it)
    {
        if ( (*it)->type == slot_type && item->equippable )
        {
            if ( !(*it)->HasRoomFor(item) )
            {
                //(*it)->RemoveFromSlot();
                this->unequip_item((*it)->equipped_item);
                //this->unequip_item(item);
            }
            //put in slot 
            (*it)->AddToSlot(item);
            break;
        }
    }

};

void Equipment::unequip_item(Item* item)
{
    for (std::vector<Slot*>::iterator it = this->slots->begin(); it != this->slots->end(); ++it)
    {
        if ((*it)->GetEquippedItem() == item)
        {
            (*it)->RemoveFromSlot();
            (*it)->equipped_item = NULL;
            //std::cout << "removed from slot" << std::endl;
        }

    };

};

bool Equipment::is_item_equipped(Item* item)
{
    if (
            this->head->GetEquippedItem() == item ||
            this->earrings->GetEquippedItem() == item ||
            this->necklace->GetEquippedItem() == item ||

            this->chest->GetEquippedItem() == item ||
            this->left_shoulder->GetEquippedItem() == item ||
            this->right_shoulder->GetEquippedItem() == item ||

            this->left_bracer->GetEquippedItem() == item ||
            this->right_bracer->GetEquippedItem() == item ||

            this->left_glove->GetEquippedItem() == item ||
            this->right_glove->GetEquippedItem() == item ||

            this->left_ring_ring->GetEquippedItem() == item ||
            this->right_ring_ring->GetEquippedItem() == item ||
            this->left_ring_middle->GetEquippedItem() == item ||
            this->right_ring_middle->GetEquippedItem() == item ||
            this->left_ring_index->GetEquippedItem() == item ||
            this->right_ring_index->GetEquippedItem() == item ||

            this->main_weapon->GetEquippedItem() == item ||
            this->off_weapon->GetEquippedItem() == item ||

            this->left_thigh->GetEquippedItem() == item ||
            this->right_thigh->GetEquippedItem() == item ||
            this->left_shin->GetEquippedItem() == item ||
            this->right_shin->GetEquippedItem() == item ||
            this->left_foot->GetEquippedItem() == item ||
            this->right_foot->GetEquippedItem() == item
            ) 
            {
                return true;
            }
    else 
    {
        return false;
    };

};

Equipment::~Equipment()
{

    delete this->head;
    delete this->earrings;
    delete this->necklace;

    delete this->chest;
    delete this->left_shoulder;
    delete this->right_shoulder;

    delete this->left_bracer;
    delete this->right_bracer;

    delete this->left_glove;
    delete this->right_glove;

    delete this->left_ring_ring;
    delete this->right_ring_ring;
    delete this->left_ring_middle;
    delete this->right_ring_middle;
    delete this->left_ring_index;
    delete this->right_ring_index;

    delete this->main_weapon;
    delete this->off_weapon;

};

void Equipment::Update()
{
};
