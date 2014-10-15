#pragma once
#ifndef SPAWNTYPES_T_H
#define SPAWNTYPES_T_H

enum MonsterSpawnTypes {
    TrollSpawn = 1,
    JackalSpawn,
    SkeletonSpawn,
    OgreSpawn,
    BadMotherSpawn,

    HulkingMantisSpawn,
    IdolSpawn,
    ImpSpawn,
    MutantFishSpawn,
    SpinyLizardSpawn,
    CrazedCookSpawn,
    WildlingSpawn,
    SludgeSpawn,
    JumperSpawn,

    ErrorSpawn
};

enum ItemSpawnTypes {
    CorpseSpawn,
    GenericSpawn, 

    ArmorSpawn,
    WeaponSpawn,
    PotionSpawn,
    ScrollSpawn,
    WandSpawn,
    HeroSpawn,
    SpecialSpawn,

    NothingItemSpawn
};

enum WandSpawnTypes {
    TeleportWandSpawn,
    ForcePushWandSpawn,
    IlluminateWandSpawn
};

enum WeaponSpawnTypes {
    SwordSpawn,
    MaceSpawn,
    DaggerSpawn,
    TridentSpawn,
    WhipSpawn,
    KatanaSpawn,
    TomahawkSpawn,
    BroadswordSpawn,
    GreatAxeSpawn,
    HatchetSpawn,
    DarkBladeSpawn,
    NothingWeaponSpawn
};

enum ArmorSpawnTypes {
    ChainmailSpawn,
    LeatherChestSpawn,
    ShieldSpawn,
    TargetShieldSpawn,
    KiteShieldSpawn,
    HelmetSpawn,
    CrownSpawn,
    LinkedMailSpawn,
    CuirassSpawn,
    PlateMailSpawn,
    DemonhideSpawn,
    MaskSpawn,
    GrimHelmSpawn,

    NothingArmorSpawn
};

enum PotionSpawnTypes {
    HealthPotionSpawn,
    GlowingHealthPotionSpawn,
    PulsatingHealthSpawn,
    PulsatingManaSpawn,
    GlowingManaPotionSpawn,
    ManaPotionSpawn,
    NothingPotionSpawn
};

enum SpecialSpawnTypes {
    GlowingAmuletSpawn,
    GoldenRingSpawn,
    RedFireFlySpawn,
    GreenFireFlySpawn,
    BlueFireFlySpawn,
    GreyFireFlySpawn,
    NothingAmuletSpawn
};

enum EventSpawnTypes {
    SpecialHeroSpawn = 0
};
#endif
