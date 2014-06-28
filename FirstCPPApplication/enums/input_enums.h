#pragma once
#ifndef INPUT_ENUMS_H
#define INPUT_ENUMS_H

enum directions_t {
    NW=0, N, NE,
    W,    X,  E,
    SW,   S, SE,
    NO_MATCHING_DIRECTION
};

enum basic_cmds_t {
    Pickup = 0, EquipFromFloor,
    Drop,
    OpenInventory,
    CancelCast,
    Look, ActivateDoor,
    DownStairs, UpStairs,
    OpenMagic, ConfirmCast,
    OpenCharacterSheet,
    OpenHelp, OpenClassSelect,
    CenterScreenToMouse,
    NextTarget, PrevTarget,
    ToggleMusic,
    ToggleSneaking,
    CustomKey1,
    CustomKey2,
    CustomKey3,
    CustomKey4,
    CustomKey5,
    NO_MATCHING_BASIC_CMD
};


enum inventory_items_active_t {
    ExamineItem,
    EquipItem, UnequipItem,
    DropItem, EscapeMenuItem,
    UseItem,
    SortByEquippableItem,
    SortByUsableItem,
    SortByEquippedItem,
    ZapItem,
    MarkItem,
    NO_MATCHING_ITEMS_ACTIVE
};


enum generic_menu_active_t {
    EscapeGenericMenu,
    NO_MATCHING_GENERIC_MENU_ACTIVE
};


enum spells_active_t {
    ExamineSpell,
    EquipSpell, UnequipSpell,
    DropSpell, EscapeMenuSpell,
    CastSpell,
    MarkSpell,
    NO_MATCHING_SPELLS_ACTIVE
};

enum classes_active_t {
    ExamineClass,
    ChangeToClass, UnequipClass,
    DropClass, EscapeMenuClass,
    CastClass,
    NO_MATCHING_CLASSES_ACTIVE
};
#endif
