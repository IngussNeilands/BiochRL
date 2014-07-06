#include "stdafx.h"
#include "drop_handler.h"


#include "actors\actor.h"
#include "actors\Person.h"

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
    rwm.add_item(MaceSpawn, 50);
    rwm.add_item(DaggerSpawn, 50);
    rwm.add_item(HatchetSpawn, 45);
    rwm.add_item(WhipSpawn, 30);
    rwm.add_item(SwordSpawn, 25);
    rwm.add_item(TomahawkSpawn, 20);
    rwm.add_item(BroadswordSpawn, 20);
    rwm.add_item(TridentSpawn, 10);
    rwm.add_item(GreatAxeSpawn, 5);
    rwm.add_item(KatanaSpawn, 2);
    rwm.add_item(DarkBladeSpawn, 2);

    WeaponSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == SwordSpawn)
    {
        std::string description = "It looks like it can swipe left to right and up and down. Wow.";
        dropped_item = spawnEquippable("A sword", description, '/', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::grey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "sword", rng, 5, 15, 7);
    }
    else if (result == MaceSpawn)
    {
        std::string description = "It looks like your mom.";
        dropped_item = spawnEquippable("A mace", description, '/', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::sepia, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "mace", rng, 1, 5, 3);
    }
    else if (result == DaggerSpawn)
    {
        std::string description = "It cuts quickly and deeply";
        dropped_item = spawnEquippable("A dagger", description, '/', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lighterSepia, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "dagger", rng, 1, 5, 3);
    }
    else if (result == TridentSpawn)
    {
        std::string description = "It looks like it could kill a man.";
        dropped_item = spawnEquippable("A trident", description, '/', slots_t::MainHand, 14);
        dropped_item->repr->setFGColor(TCODColor::desaturatedBlue, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "trident", rng, 4, 20, 10);
    }
    else if (result == WhipSpawn)
    {
        std::string description = "It looks as if it had been used on something recently.";
        dropped_item = spawnEquippable("A whip", description, '&', slots_t::MainHand, 5);
        dropped_item->repr->setFGColor(TCODColor::darkerOrange, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "whip", rng, 5, 10, 6);
    }
    else if (result == KatanaSpawn)
    {
        std::string description = "It looks very sharp.";
        dropped_item = spawnEquippable("A katana", description, '\\', slots_t::MainHand, 7);
        dropped_item->repr->setFGColor(TCODColor::lightestHan, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "katana", rng, 10, 30, 11);
    }
    else if (result == TomahawkSpawn)
    {
        std::string description = "It looks as if both edges have spilled blood.";
        dropped_item = spawnEquippable("A tomahawk", description, '(', slots_t::MainHand, 9);
        dropped_item->repr->setFGColor(TCODColor::darkerFlame, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "tomahawk", rng, 10, 20, 11);
    }
    else if (result == BroadswordSpawn)
    {
        std::string description = "It looks like a sword, only broader.";
        dropped_item = spawnEquippable("A broadsword", description, '\\', slots_t::MainHand, 12);
        dropped_item->repr->setFGColor(TCODColor::darkerGrey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "broadsword", rng, 8, 17, 11);
    }
    else if (result == GreatAxeSpawn)
    {
        std::string description = "Both edges look as sharp as stalker's tongue.";
        dropped_item = spawnEquippable("A greataxe", description, '(', slots_t::MainHand, 20);
        dropped_item->repr->setFGColor(TCODColor::darkerGrey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "greataxe", rng, 15, 30, 13);
    }
    else if (result == HatchetSpawn)
    {
        std::string description = "It looks like it could barely cut through a tree.";
        dropped_item = spawnEquippable("A hatchet", description, '(', slots_t::MainHand, 7);
        dropped_item->repr->setFGColor(TCODColor::grey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "hatchet", rng, 5, 15, 7);
    }
    else if (result == DarkBladeSpawn)
    {
        std::string description = "It looks as if it blends in with the shadows.";
        dropped_item = spawnEquippable("A darkblade", description, '/', slots_t::OffHand, 3);
        dropped_item->repr->setFGColor(TCODColor::darkestGrey, true, false, true);
        dropped_item->set_and_name_for_dmg("A", "darkblade", rng, 10, 20, 11);
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
    rwm.add_item(LeatherChestSpawn, 50);
    rwm.add_item(ChainmailSpawn, 25);
    rwm.add_item(CuirassSpawn, 20);
    rwm.add_item(LinkedMailSpawn, 10);
    rwm.add_item(PlateMailSpawn, 10);
    rwm.add_item(DemonhideSpawn, 2);

    rwm.add_item(ShieldSpawn, 50);
    rwm.add_item(TargetShieldSpawn, 25);
    rwm.add_item(KiteShieldSpawn, 20);

    rwm.add_item(HelmetSpawn, 50);
    rwm.add_item(MaskSpawn, 30);
    rwm.add_item(CrownSpawn, 10);
    rwm.add_item(GrimHelmSpawn, 10);


    ArmorSpawnTypes result = rwm.get_item(Game::item_spawn_rng);
    if (result == ChainmailSpawn)
    {
        std::string description = "It looks like it's made up of smaller more delicate pieces.";
        dropped_item = spawnEquippable("Chainmail", description, '&', slots_t::Chest, 10);
        dropped_item->repr->setFGColor(TCODColor::grey, true, false, true);
        dropped_item->set_and_name_for_arm("Some", "Chainmail", rng, 3, 8, 3);
    }
    else if (result == LeatherChestSpawn)
    {
        std::string description = "It looks like it's made up of leather hide.";
        dropped_item = spawnEquippable("Leather Chestpiece", description, '#', slots_t::Chest, 10);
        dropped_item->repr->setFGColor(TCODColor::darkestRed, true, false, true);
        dropped_item->set_and_name_for_arm("Some", "Leather Chestpiece", rng, 2, 6, 2);
    }
    else if (result == ShieldSpawn)
    {
        std::string description = "It looks like it can take a few hits.";
        dropped_item = spawnEquippable("A shield", description, ']', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        dropped_item->set_and_name_for_arm("A", "shield", rng, 1, 5, 3);
    }
    else if (result == TargetShieldSpawn)
    {
        std::string description = "It looks like it can block several hits.";
        dropped_item = spawnEquippable("A target shield", description, ']', slots_t::OffHand, 6);
        dropped_item->repr->setFGColor(TCODColor::lightestGrey, true, false, true);
        dropped_item->set_and_name_for_arm("A", "target shield", rng, 3, 7, 3);
        
    }
    else if (result == KiteShieldSpawn)
    {
        std::string description = "It looks large and sturdy.";
        dropped_item = spawnEquippable("A kite shield", description, '#', slots_t::OffHand, 5);
        dropped_item->repr->setFGColor(TCODColor::sepia, true, false, true);
        dropped_item->set_and_name_for_arm("A", "kite shield", rng, 3, 9, 3);

    }
    else if (result == HelmetSpawn)
    {
        std::string description = "It looks sturdy.";
        dropped_item = spawnEquippable("A helmet", description, '^', slots_t::Head, 5);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        dropped_item->set_and_name_for_arm("A", "helmet", rng, 1, 7, 3);

    }
    else if (result == LinkedMailSpawn)
    {
        std::string description = "It looks like several pieces of steel are sown together.";
        dropped_item = spawnEquippable("Some linked mail", description, '#', slots_t::Chest, 5);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        dropped_item->set_and_name_for_arm("Some", "linked mail", rng, 4, 11, 5);

    }
    else if (result == CuirassSpawn)
    {
        std::string description = "It looks nearly sturdy.";
        dropped_item = spawnEquippable("Some cuirass", description, '#', slots_t::Chest, 5);
        dropped_item->repr->setFGColor(TCODColor::darkSepia, true, false, true);
        dropped_item->set_and_name_for_arm("Some", "cuirass", rng, 2, 9, 3);

    }
    else if (result == PlateMailSpawn)
    {
        std::string description = "It looks hella sturdy.";
        dropped_item = spawnEquippable("Some platemail", description, '#', slots_t::Chest, 5);
        dropped_item->repr->setFGColor(TCODColor::lightGrey, true, false, true);
        dropped_item->set_and_name_for_arm("Some", "platemail", rng, 7, 14, 6);

    }
    else if (result == DemonhideSpawn)
    {
        std::string description = "It looks like it was forged in the shadows.";
        dropped_item = spawnEquippable("A Demonhide chestpiece", description, '#', slots_t::Chest, 5);
        dropped_item->repr->setFGColor(TCODColor::darkestCrimson, true, false, true);
        dropped_item->set_and_name_for_arm("A", "Demonhide chestpiece", rng, 10, 20, 13);

    }
    else if (result == MaskSpawn)
    {
        std::string description = "It looks like it'd be perfect for hiding your face from others.";
        dropped_item = spawnEquippable("A mask", description, '^', slots_t::Head, 5);
        dropped_item->repr->setFGColor(TCODColor::lightOrange, true, false, true);
        dropped_item->set_and_name_for_arm("A", "mask", rng, 2, 6, 5);

    }
    else if (result == GrimHelmSpawn)
    {
        std::string description = "It looks this wouldn't help you make any more allies.";
        dropped_item = spawnEquippable("A grimhelm", description, '^', slots_t::Head, 7);
        dropped_item->repr->setFGColor(TCODColor::darkerRed, true, false, true);
        dropped_item->set_and_name_for_arm("A", "grimhelm", rng, 1, 7, 3);

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
    rwm.add_item(GlowingHealthPotionSpawn, 25);
    rwm.add_item(PulsatingHealthSpawn, 10);

    rwm.add_item(ManaPotionSpawn, 50);
    rwm.add_item(GlowingManaPotionSpawn, 25);
    rwm.add_item(PulsatingManaSpawn, 10);

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
        dropped_item->attr_effect->health_regen_interval = - floor((double)health/2);
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
        dropped_item->repr->setFGColor(TCODColor::lighterBlue, true, false, true);

        //mana regen
        int mana = rng->getInt(5, 50, 15);
        dropped_item->attr_effect->mana_current_val = mana;
        dropped_item->attr_effect->mana_max_val = mana;
    }
    else if (result == PulsatingManaSpawn)
    {
        std::string description = "It looks like an especially good time.";
        dropped_item = spawnUsable("A pulsating blue potion", description, '!', slots_t::NoSlot, 1);
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
        dropped_item->uses = 99999;
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
        dropped_item->uses = 99999;
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
        dropped_item = spawnEquippable("A bottle of auburn fireflies", description, '0', slots_t::Fingers, 1);
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

Item* DropHandler::handle_for_actor(Actor* actor)
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
        dropped_item = actor->create_corpse();
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
