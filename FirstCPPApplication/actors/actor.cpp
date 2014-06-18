#include "stdafx.h"

#include <fstream>
#include <time.h>

// #include "libtcod_cpp_hpp\libtcod.hpp"

#include "actor.h"
#include "Person.h"

#include "thinker.h"
#include "inventory.h"
#include "equipment.h"
#include "tile.h"
#include "attribute.h"
#include "attribute_container.h"
#include "Representation.h"
#include "item.h"
#include "attr_effect.h"
#include "ui.h"
#include "messages.h"
#include "game.h"
#include "class.h"
#include <enums\spawntypes_t.h>
#include <randsys.h>
#include <utils.h>
#include <spells.h>
#include "map.h"
#include "combat.h"
#include <color_utils.h>


int Actor::pack_size = 10;
int Actor::champ_chance = 10;
int Actor::preferred_pack_size = 10;

std::vector<IClass*>* Actor::actor_class_choices = new std::vector<IClass*>();

Actor::Actor()
{
    this->cls_name = "Actor";
    this->name = "Unset Actor name";

    this->img_path = get_data_path()+"img/Troll_Icon_01.png";

    this->xp = 0;
    this->xp_this_level = 0;
    this->xp_required = 100;
    this->xp_value = 10;
    this->level = 1;

    this->is_champion = false;
    this->is_hero = false;
    this->timed_attr_effects = new std::vector<TimedEffect*>;
    this->timed_spell_effects = new std::vector<TimedEffect*>;

    this->my_tile = NULL;

    this->representation = new Representation;
    this->inventory = new Inventory;
    this->inventory->master = this;
    this->attrs = new AttributeContainer;
    this->attrs->owner = this;
    this->spells = new std::vector<Spell*>;

    this->actors_in_sight = new std::vector<Actor*>;

    this->thinker = new Thinker;
    this->thinker->master = this;

    this->is_active = true;

    this->is_moving_left = false;
    this->is_moving_right = false;
    this->is_moving_up = false;
    this->is_moving_down = false;

    this->l_path = NULL;
    this->has_attacked = false;
};

TCODImage* Actor::get_image()
{
    TCODImage* img = new TCODImage(this->img_path.c_str());
    img->setKeyColor(TCODColor(255, 0, 255)); //XNA Pink
    return img;
};

Actor::~Actor()
{
    delete this->timed_attr_effects;
    delete this->timed_spell_effects;

    delete this->actors_in_sight;
    delete this->inventory;
    delete this->representation;
    delete this->attrs;
    delete this->spells;
    delete this->thinker;
};

void Actor::resetIsMoving()
{
    this->is_moving_up = false; 
    this->is_moving_right = false;
    this->is_moving_down = false;
    this->is_moving_left = false;
};

std::string Actor::GetName()
{
    return name;
};

const char* Actor::GetNameC()
{
    return name.c_str();
};

bool Actor::IsActorInSight(Actor* actor)
{

    std::vector<Actor*>::iterator it;
    it = std::find(this->actors_in_sight->begin(), this->actors_in_sight->end(),  actor);
    if (it != this->actors_in_sight->end()) 
    {
        //std::cout << "hes in sight" << std::endl;
        return true; 
    }
    else 
    {
        return false; 
    }
};

void Actor::mark_as_unseen(Actor * actor)
{
    // std::cout << "removing from sight" << std::endl;
    std::vector<Actor*>* ais = this->actors_in_sight;
    //if the actor is in sight and isnt this actor:
    if (actor == this) 
    {
        return;
    }
    ais->erase(std::remove(ais->begin(), ais->end(), actor), ais->end());

}
void Actor::mark_as_seen(int x, int y, Actor * actor)
{
    std::vector<Actor*>* ais = this->actors_in_sight;
    //if the actor is in sight and isnt this actor:
    if (actor == this) 
    {
        return;
    }
    if (std::find(ais->begin(), ais->end(), actor) != ais->end())
    {
    }
    else 
    {
        this->actors_in_sight->push_back(actor);
    }

}

void Actor::putPerson( Tile * next_tile, int new_x, int new_y)
{	//puts a person on a tile, resets the old tile

    if (my_tile != NULL){
        my_tile->makeUnoccupied(this); 
    }

    if (next_tile != NULL){
        this->my_tile = next_tile;
        next_tile->makeOccupied(this); 
    }
    else if (next_tile == NULL)
    {
        std::cout << "next tile is null" << std::endl;
    }
    else
    {
        std::cout << "else, this shouldn't be a possiblity" << std::endl;
    }

    if (new_x != -1 && new_y != -1)
    {
        this->x = new_x;
        this->y = new_y;
    }

};

void Actor::pickUpItem(Item* item)
{

    //add to inventory
    this->inventory->add_item(item);

    // remove from Tiles
    this->my_tile->pick_up_item(item);

};

bool Actor::has_attributes()
{
    return this->attrs != NULL;
};

Item* Actor::CreateCorpse()
{
    Item* corpse = new Item;
    corpse->repr->repr = '%';
    corpse->repr->setFGColor(*this->representation->fg_color, true, false, true);

    // std::string colored_corpse = colfg(TCODColor::red, " corpse");
    // corpse->name =  this->cls_name + colored_corpse;
    corpse->name = this->cls_name + " corpse";
    corpse->slot_type = slots_t::NoSlot;
    corpse->attr_effect->set_all_vals_to(0);
    corpse->attr_effect->health_regen_interval=1.25;
    corpse->attr_effect->health_regen_rate=1;
    corpse->attr_effect->hunger_current_val=-50;
    corpse->usable = true;
    corpse->equippable = false;

    return corpse;
};

int* Actor::get_direction_heading()
{
    int dir_array[2];

    //handle x coord
    if (this->is_moving_left) { dir_array[0] = -1; }
    else if (this->is_moving_right) { dir_array[0] =  1; }
    else { dir_array[0] = 0; };

    //handle y coord
    if (this->is_moving_up) { dir_array[1] = -1; }
    else if (this->is_moving_down) { dir_array[1] =  1; }
    else { dir_array[1] = 0; };

    return dir_array;
};

Item* spawnItemBase(std::string name, std::string description, char repr, slots_t slot, int weight)
{
        Item* dropped_item = new Item;
        dropped_item->repr->repr = repr;
        dropped_item->name = colfg(TCODColor::lightestGreen, name);
        dropped_item->description = description;
        dropped_item->slot_type = slot;
        dropped_item->weight = weight;
        dropped_item->attr_effect->set_all_vals_to(0);
        return dropped_item;
};

Item* spawnEquippable(std::string name, std::string description, char repr, slots_t slot, int weight)
{
    Item* dropped_item = spawnItemBase(name, description, repr, slot, weight);
    dropped_item->equippable = true;
    return dropped_item;
};

Item* spawnUsable(std::string name, std::string description, char repr, slots_t slot, int weight)
{
    Item* dropped_item = spawnItemBase(name, description, repr, slot, weight);
    dropped_item->usable = true;
    return dropped_item;
};

Item* spawnWeapon()
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<WeaponSpawnTypes> rwm = RandomWeightMap<WeaponSpawnTypes>();
    rwm.add_item(SwordSpawn, 25);
    rwm.add_item(MaceSpawn, 50);
    rwm.add_item(DaggerSpawn, 50);
    rwm.add_item(TridentSpawn, 10);
    rwm.add_item(WhipSpawn, 30);
    rwm.add_item(KatanaSpawn, 2);

    WeaponSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == SwordSpawn)
    {
        std::string description = "It looks like it can swipe left to right and up and down. Wow.";
        dropped_item = spawnEquippable("A sword", description, '/', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::grey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "sword", rng, 5, 15, 7);
        // dropped_item->attr_effect->set_rng_damage(rng, 5, 15, 8);
    }
    else if (result == MaceSpawn)
    {
        std::string description = "It looks like your mom.";
        dropped_item = spawnEquippable("A mace", description, '/', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::sepia, true, false, true);
        // dropped_item->attr_effect->set_rng_damage(rng, 1, 5, 3);
        dropped_item->set_and_name_for_dmg("A", "mace", rng, 1, 5, 3);
    }
    else if (result == DaggerSpawn)
    {
        std::string description = "It cuts quickly and deeply";
        dropped_item = spawnEquippable("A dagger", description, '/', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lighterSepia, true, false, true);
        // dropped_item->attr_effect->set_rng_damage(rng, 1, 5, 3);
        dropped_item->set_and_name_for_dmg("A", "dagger", rng, 1, 5, 3);
    }
    else if (result == TridentSpawn)
    {
        std::string description = "It looks like it could kill a man.";
        dropped_item = spawnEquippable("A trident", description, '/', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::desaturatedBlue, true, false, true);
        // dropped_item->attr_effect->set_rng_damage(rng, 4, 20, 5);
        dropped_item->set_and_name_for_dmg("A", "trident", rng, 4, 20, 5);
    }
    else if (result == WhipSpawn)
    {
        std::string description = "It looks as if it had been used on something recently.";
        dropped_item = spawnEquippable("A whip", description, '&', slots_t::MainHand, 5);
        dropped_item->repr->setFGColor(TCODColor::darkerOrange, true, false, true);
        // dropped_item->attr_effect->set_rng_damage(rng, 4, 9, 4);
        dropped_item->set_and_name_for_dmg("A", "whip", rng, 5, 10, 6);
    }
    else if (result == KatanaSpawn)
    {
        std::string description = "It looks very sharp.";
        dropped_item = spawnEquippable("A katana", description, '\\', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::lightestHan, true, false, true);
        // dropped_item->attr_effect->set_rng_damage(rng, 10, 20, 11);
        dropped_item->set_and_name_for_dmg("A", "katana", rng, 10, 30, 11);
    }
    else
    {
        assert(false && "RandomWeightMap returned invalid weapon variable");
    };

    return dropped_item;
};

Item* spawnArmor()
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<ArmorSpawnTypes> rwm = RandomWeightMap<ArmorSpawnTypes>();
    rwm.add_item(ChainmailSpawn, 25);
    rwm.add_item(LeatherChestSpawn, 50);
    rwm.add_item(ShieldSpawn, 50);
    rwm.add_item(TargetShieldSpawn, 25);
    rwm.add_item(HelmetSpawn, 50);
    rwm.add_item(CrownSpawn, 10);

    ArmorSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == ChainmailSpawn)
    {
        std::string description = "It looks like it's made up of smaller more delicate pieces.";
        dropped_item = spawnEquippable("Chainmail", description, '&', slots_t::Chest, 10);
        dropped_item->repr->setFGColor(TCODColor::grey, true, false, true);
        // dropped_item->attr_effect->set_rng_armor(rng, 3, 8, 3);
        dropped_item->set_and_name_for_arm("Some", "Chainmail", rng, 3, 8, 3);
    }
    else if (result == LeatherChestSpawn)
    {
        std::string description = "It looks like it's made up of leather hide.";
        dropped_item = spawnEquippable("Leather Chestpiece", description, '&', slots_t::Chest, 10);
        dropped_item->repr->setFGColor(TCODColor::darkestRed, true, false, true);
        // dropped_item->attr_effect->set_rng_armor(rng, 1, 5, 2);
        dropped_item->set_and_name_for_arm("Some", "Leather Chestpiece", rng, 2, 6, 2);
    }
    else if (result == ShieldSpawn)
    {
        std::string description = "It looks like it can take a few hits.";
        dropped_item = spawnEquippable("A shield", description, ']', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        // dropped_item->attr_effect->set_rng_armor(rng, 1, 5, 3);
        dropped_item->set_and_name_for_arm("A", "shield", rng, 1, 5, 3);
    }
    else if (result == TargetShieldSpawn)
    {
        std::string description = "It looks like it can block several hits.";
        dropped_item = spawnEquippable("A target shield", description, ']', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lightestGrey, true, false, true);
        // dropped_item->attr_effect->set_rng_armor(rng, 3, 7, 3);
        dropped_item->set_and_name_for_arm("A", "target shield", rng, 3, 7, 3);
        
    }
    else if (result == HelmetSpawn)
    {
        std::string description = "It looks sturdy.";
        dropped_item = spawnEquippable("A helmet", description, '^', slots_t::Head, 5);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        // dropped_item->attr_effect->set_rng_armor(rng, 1, 5, 3);
        dropped_item->set_and_name_for_arm("A", "helmet", rng, 1, 7, 3);

    }
    else if (result == CrownSpawn)
    {
        std::string description = "It looks shiny.";
        dropped_item = spawnEquippable("A crown", description, '^', slots_t::Head, 5);
        dropped_item->repr->setFGColor(TCODColor::gold, true, false, true);
        int armor = rng->getInt(1, 5, 3);
        int health = rng->getInt(1, 5, 3);
        int mana = rng->getInt(1, 5, 3);
        dropped_item->attr_effect->armor_current_val = armor;
        dropped_item->attr_effect->armor_max_val = armor;
        dropped_item->attr_effect->health_current_val = health;
        dropped_item->attr_effect->health_max_val = health;
        dropped_item->attr_effect->mana_current_val = mana;
        dropped_item->attr_effect->mana_max_val = mana;
    }
    else 
    {
        assert(false && "RandomWeightMap returned invalid armor variable");
    };

    return dropped_item;
};

Item* spawnPotion()
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<PotionSpawnTypes> rwm = RandomWeightMap<PotionSpawnTypes>();
    rwm.add_item(HealthPotionSpawn, 50);
    rwm.add_item(GlowingHealthPotionSpawn, 30);
    rwm.add_item(PulsatingHealthSpawn, 10);
    rwm.add_item(ManaPotionSpawn, 50);
    rwm.add_item(GlowingManaPotionSpawn, 25);

    PotionSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == HealthPotionSpawn)
    {

        std::string description = "It looks like it's safe to drink.";
        dropped_item = spawnUsable("A green potion", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightGreen, true, false, true);

        //health restore
        int health = rng->getInt(10, 100, 25);
        dropped_item->attr_effect->health_current_val = health;
    }
    else if (result == GlowingHealthPotionSpawn)
    {
        std::string description = "It looks like it's probably safe to drink.";
        dropped_item = spawnUsable("A glowing green potion", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lighterGreen, true, false, true);

        //health restore
        int health = rng->getInt(5, 50, 15);
        dropped_item->attr_effect->health_current_val = health;
        dropped_item->attr_effect->health_max_val = health;
    }
    else if (result == PulsatingHealthSpawn)
    {

        std::string description = "It looks like could be safe to drink.";
        dropped_item = spawnUsable("A pulsating green potion", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightestGreen, true, false, true);

        //health restore
        int health = rng->getInt(1, 5, 1);
        dropped_item->attr_effect->health_regen_rate = health;
        dropped_item->attr_effect->health_regen_interval = -floor((double)health/2);
    }
    else if (result == ManaPotionSpawn)
    {
        std::string description = "It looks like a small bit of essence is trapped inside. Better drink it.";
        dropped_item = spawnUsable("A blue potion", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightBlue, true, false, true);

        //health restore
        int mana = rng->getInt(1, 50, 10);
        dropped_item->attr_effect->mana_current_val = mana;
    }
    else if (result == GlowingManaPotionSpawn)
    {
        std::string description = "It looks like a good time.";
        dropped_item = spawnUsable("A glowing blue potion", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightestBlue, true, false, true);

        //mana regen
        int mana = rng->getInt(1, 5, 1);
        dropped_item->attr_effect->mana_regen_rate = mana;
        dropped_item->attr_effect->mana_regen_interval = -floor((double)mana/2);
    }
    else
    {
        assert(false && "RandomWeightMap returned invalid potion variable");
    };

    return dropped_item;
};

Item* spawnWand()
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<WandSpawnTypes> rwm = RandomWeightMap<WandSpawnTypes>();
    rwm.add_item(TeleportWandSpawn, 14);
    rwm.add_item(ForcePushWandSpawn, 50);
    WandSpawnTypes result = rwm.get_item(Game::item_spawn_rng);

    if (result == TeleportWandSpawn)
    {

        std::string description = "It flickers in and out of existence.";
        dropped_item = spawnUsable("A shimmering wand", description, '`', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightGreen, true, false, true);

        dropped_item->spell_effect = new TeleportSelfSpell;
        //health restore
        // int health = rng->getInt(10, 100, 25);
        dropped_item->attr_effect->health_current_val = 1;
    }
    else if (result == ForcePushWandSpawn)
    {
        std::string description = "It flutters quite a bit for a piece of bark.";
        dropped_item = spawnUsable("A moving wand", description, '`', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lighterGreen, true, false, true);

        dropped_item->spell_effect = new LaunchOtherSpell;
        //health restore
        // int health = rng->getInt(5, 50, 15);
        // dropped_item->attr_effect->health_current_val = health;
        dropped_item->attr_effect->health_max_val = 1;
    }
    else if (result == PulsatingHealthSpawn)
    {

        std::string description = "It looks like could be safe to drink.";
        dropped_item = spawnUsable("A pulsating green wand", description, '!', slots_t::NoSlot, 1);
        dropped_item->repr->setFGColor(TCODColor::lightestGreen, true, false, true);

        //health restore
        int health = rng->getInt(1, 5, 1);
        dropped_item->attr_effect->health_regen_rate = health;
        dropped_item->attr_effect->health_regen_interval = -floor((double)health/2);
    }
    else
    {
        assert(false && "RandomWeightMap returned invalid wand variable");
    };

    return dropped_item;
};

Item* spawnSpecial()
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<SpecialSpawnTypes> rwm = RandomWeightMap<SpecialSpawnTypes>();
    rwm.add_item(GlowingAmuletSpawn, 50);
    rwm.add_item(GoldenRingSpawn, 10);
    rwm.add_item(RedFireFlySpawn, 15);
    rwm.add_item(GreenFireFlySpawn, 15);
    rwm.add_item(BlueFireFlySpawn, 15);
    rwm.add_item(GreyFireFlySpawn, 15);

    SpecialSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == GlowingAmuletSpawn)
    {

        std::string description = "It glows.";
        dropped_item = spawnEquippable("A amulet", description, '|', slots_t::Neck, 2);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);

        //dagger damage
        int armor = rng->getInt(1, 5, 2);
        int damage = rng->getInt(1, 5, 2);
        int mana = rng->getInt(1, 5, 2);
        dropped_item->attr_effect->armor_current_val = armor;
        dropped_item->attr_effect->armor_max_val = armor;
        dropped_item->attr_effect->damage_current_val = damage;
        dropped_item->attr_effect->damage_max_val = damage;
        dropped_item->attr_effect->mana_regen_rate = mana;
        dropped_item->attr_effect->mana_regen_interval = mana;

    }
    else if (result == GoldenRingSpawn)
    {

        std::string description = "It glows a golden hue.";
        dropped_item = spawnEquippable("A golden ring", description, '|', slots_t::Fingers, 2);
        dropped_item->repr->setFGColor(TCODColor::gold, true, false, true);

        //dagger damage
        int armor = rng->getInt(1, 5, 2);
        int health = rng->getInt(10, 50, 20);
        dropped_item->attr_effect->armor_current_val = armor;
        dropped_item->attr_effect->armor_max_val = armor;
        dropped_item->attr_effect->health_current_val = health;
        dropped_item->attr_effect->health_max_val = health;

    }
    else if (result == RedFireFlySpawn)
    {

        std::string description = "Auburn Fireflies in a bottle";
        dropped_item = spawnEquippable("A bottle of red fireflies", description, '0', slots_t::Fingers, 1);
        dropped_item->repr->setFGColor(TCODColor::fuchsia, true, false, true);
        dropped_item->usable = true;
        dropped_item->equippable = false;

        //dagger damage
        // int armor = rng->getInt(1, 5, 2);
        int damage = rng->getInt(10, 50, 20);
        dropped_item->attr_effect->damage_current_val = damage;
        dropped_item->attr_effect->damage_max_val = damage;
        dropped_item->attr_effect->duration = 15;


    }
    else if (result == BlueFireFlySpawn)
    {

        std::string description = "Azure Fireflies in a bottle";
        dropped_item = spawnEquippable("A bottle of azure fireflies", description, '0', slots_t::Fingers, 1);
        dropped_item->repr->setFGColor(TCODColor::azure, true, false, true);
        dropped_item->usable = true;
        dropped_item->equippable = false;

        //dagger damage
        // int armor = rng->getInt(1, 5, 2);
        int mana = rng->getInt(10, 50, 20);
        dropped_item->attr_effect->mana_current_val = mana;
        dropped_item->attr_effect->mana_max_val = mana;
        dropped_item->attr_effect->duration = 15;
    }
    else if (result == GreenFireFlySpawn)
    {

        std::string description = "Celadon Fireflies in a bottle";
        dropped_item = spawnEquippable("A bottle of celadon fireflies", description, '0', slots_t::Fingers, 1);
        dropped_item->repr->setFGColor(TCODColor::celadon, true, false, true);
        dropped_item->usable = true;
        dropped_item->equippable = false;

        //dagger damage
        // int armor = rng->getInt(1, 5, 2);
        int health = rng->getInt(10, 50, 20);
        dropped_item->attr_effect->health_current_val = health;
        dropped_item->attr_effect->health_max_val = health;
        dropped_item->attr_effect->duration = 15;
    }
    else if (result == GreyFireFlySpawn)
    {

        std::string description = "Sepia Fireflies in a bottle";
        dropped_item = spawnEquippable("A bottle of sepia fireflies", description, '0', slots_t::Fingers, 1);
        dropped_item->repr->setFGColor(TCODColor::sepia, true, false, true);
        dropped_item->usable = true;
        dropped_item->equippable = false;

        //dagger damage
        // int armor = rng->getInt(1, 5, 2);
        int armor = rng->getInt(5, 20, 10);
        dropped_item->attr_effect->armor_current_val = armor;
        dropped_item->attr_effect->armor_max_val = armor;
        dropped_item->attr_effect->duration = 15;
    }
    else
    {
        assert(false && "RandomWeightMap returned invalid amulet variable");
    };

    return dropped_item;
};

Item* spawnItem(Actor* actor)
{
    Item* dropped_item;
    TCODRandom *rng = Game::item_spawn_rng;

    RandomWeightMap<ItemSpawnTypes> rwm = RandomWeightMap<ItemSpawnTypes>();
    if (!actor->is_hero)
    {
        rwm.add_item(ArmorSpawn, 500);
        rwm.add_item(WeaponSpawn, 500);
        rwm.add_item(PotionSpawn, 300);
        rwm.add_item(WandSpawn, 10);
        rwm.add_item(SpecialSpawn, 100);
    }
    else
    {
        rwm.add_item(WandSpawn, 10);
        rwm.add_item(SpecialSpawn, 100);
    };

    ItemSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == WeaponSpawn)
    {
        dropped_item = spawnWeapon();
    }
    else if (result == ArmorSpawn)
    {
        dropped_item = spawnArmor();
    }
    else if (result == PotionSpawn)
    {
        dropped_item = spawnPotion();
    }
    else if (result == WandSpawn)
    {
        dropped_item = spawnWand();
    }
    else if (result == SpecialSpawn)
    {
        dropped_item = spawnSpecial();
    }
    else
    {
        dropped_item = NULL;
    }

    return dropped_item;
};

Item* Actor::item_drop_handler(Actor* actor)
{
    RandomWeightMap<ItemSpawnTypes> rwm = RandomWeightMap<ItemSpawnTypes>();
    if (actor->is_champion)
    {
        rwm.add_item(GenericSpawn, 30);
    }
    else if (actor->is_hero)
    {
        rwm.add_item(GenericSpawn, 30);

    }
    else
    {
        rwm.add_item(CorpseSpawn, 50);
        rwm.add_item(GenericSpawn, 30);
        rwm.add_item(NothingItemSpawn, 90);
    };

    ItemSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    TCODRandom *rng = Game::item_spawn_rng;
    Item* dropped_item = NULL;
    if (result == CorpseSpawn)
    {
        dropped_item = actor->CreateCorpse();
    }
    else if (result == GenericSpawn)
    {
        dropped_item = spawnItem(actor);
    }
    else  if (result == NothingItemSpawn)
    {
        //nothing happens between result of 15 and 60
    }
    else 
    {
        assert(false && "RandomWeightMap returned invalid variable");
    };

    return dropped_item;
};

void Actor::ScorePrintout()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y_%Ih%Mm%Ss",timeinfo);
    std::string str(buffer);

    std::string filepath = (get_data_path()+"necropolis_"+str+".log" );
    std::ofstream necro(filepath.c_str(), std::ofstream::out);

    //game version
    necro << Game::get_version() << std::endl;
    necro  << std::endl;
    //xp == score
    necro << "XP: " << Game::player->xp << std::endl;
    necro << "LEVEL: " << Game::player->level << std::endl;
    necro << "CLASS: " << Game::player->actor_class->name << std::endl;

    necro << std::endl;
    necro << "FLOOR: " << Game::current_map->depth << std::endl;
    necro << "TURNS: " << Game::turn_count << std::endl;
    necro << std::endl;
    //stats 
    necro << "KILLS: " << Game::stats->monsters_killed << std::endl;
    necro << "SPELLS CAST: " << Game::stats->spells_cast << std::endl;
    necro << "ITEMS USED:" << Game::stats->items_used << std::endl;
    necro << "PHYS DMG DEALT:" << Game::stats->damage_dealt << std::endl;
    necro << "PHYS DMG TAKEN:" << Game::stats->damage_taken << std::endl;
    necro << std::endl;

    necro << "----- STATS -----"  << std::endl;
    necro << Game::player->attrs->PrettyPrint() << std::endl;
    necro  << std::endl;

    //equipment
    necro << std::endl;

    //inventory
    necro << "----- INVENTORY -----"  << std::endl;
    for (auto it = Game::player->inventory->items->begin(); it!=Game::player->inventory->items->end(); it++)
    {
        necro << (*it)->name << std::endl;
        necro << (*it)->description << std::endl;
        necro << (*it)->attr_effect->oneline_str_colorless().c_str() << std::endl << std::endl;
    }
    necro  << std::endl;

    necro.close();
};

void Actor::Die()
{
    Item* dropped_item = NULL;
    dropped_item = Actor::item_drop_handler(this);

    if (dropped_item != NULL)
    {
        this->my_tile->place_item_down(dropped_item);
    }

    if (this->combat != NULL)
    {
	Combat* last_attacker = this->combat->GetLastAttacker();
        //if (last_attacker != NULL && last_attacker->master == Game::player)
        if (last_attacker->master == Game::player)
        {
            Game::stats->monsters_killed++;
        };
    };

    Representation* repr = this->my_tile->get_representation();
    Representation* new_repr = new FloorRepresentation;
    new_repr->repr = repr->repr;
    new_repr->setFGColor((*repr->fg_color)+(TCODColor::darkerRed*0.3), true, true, true);
    this->my_tile->set_representation(new_repr);

    this->my_tile->set_description("There is some blood on the ground.");

    //remove master from ai update list
    this->is_active = false;
    this->putPerson(NULL, NULL, NULL);

    //FIXME this is because people can still 'see' the dead dude. removing only
    //from players view, will need to remove from everyone elses at some point
    Game::player->mark_as_unseen(this);

    //destroy ai
    if (this->thinker != NULL)
    {
        delete this->thinker;
    };

    new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(TCODColor::darkerRed, "%s died!"), this->name.c_str());

    int multiplier = 1;
    if (this == (Actor*)Game::player)
    {
        TCODSystem::setFps(0);
        for (int fade=255*multiplier; fade >= 0; fade --) {
            if (fade % multiplier == 0)
            {
                TCODConsole::setFade(fade/multiplier,TCODColor::red);
                TCODConsole::flush();
                printf("YOU'RE DEAD GIVE UP\n");
            };
        }

        std::cout << "Death log is being prepared..." << std::endl;
        this->ScorePrintout();

        TCODSystem::setFps(Game::fps_limit);
        exit(1);

    };

};
