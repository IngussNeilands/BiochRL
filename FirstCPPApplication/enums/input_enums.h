#pragma once
#ifndef INPUT_ENUMS_H
#define INPUT_ENUMS_H

enum directions_t {
    NO_MATCHING_DIRECTION = -1,
    NW=0, N, NE,
    W,    X,  E,
    SW,   S, SE,
};

enum basic_cmds_t {
    NO_MATCHING_BASIC_CMD = -1,
    Pickup = 0, EquipFromFloor,
    Drop,
    OpenInventory,
    CancelCast,
    Look, Activate,
    DownStairs, UpStairs,
    OpenMagic, ConfirmCast,
    OpenCharacterSheet,
    OpenHelp, OpenClassSelect,
    ListCustomKeys,
    CenterScreenToMouse,
    NextTarget, PrevTarget,
    ToggleMusic,
    ToggleSneaking,
    CustomKey1,
    CustomKey2,
    CustomKey3,
    CustomKey4,
    CustomKey5,
    LAST_BASIC_ACTIVE
};


enum inventory_items_active_t {
    NO_MATCHING_ITEMS_ACTIVE = -1,
    ExamineItem = 0,
    EquipItem, UnequipItem,
    DropItem, EscapeMenuItem,
    UseItem,
    SortByEquippableItem,
    SortByUsableItem,
    SortByEquippedItem,
    ZapItem,
    MarkItem,
    LAST_ITEMS_ACTIVE
};


enum generic_menu_active_t {
    NO_MATCHING_GENERIC_MENU_ACTIVE= -1,
    EscapeGenericMenu,
    ActivateGenericMenuItem,
    ExamineGenericMenuItem,
    LAST_GENERIC_MENU_ACTIVE
};


enum spells_active_t {
    NO_MATCHING_SPELLS_ACTIVE= -1,
    ExamineSpell,
    EquipSpell, UnequipSpell,
    DropSpell, EscapeMenuSpell,
    CastSpell,
    MarkSpell,
    LAST_SPELLS_ACTIVE
};

enum classes_active_t {
    NO_MATCHING_CLASSES_ACTIVE = -1,
    ExamineClass,
    ChangeToClass, UnequipClass,
    DropClass, EscapeMenuClass,
    CastClass,
    LAST_CLASSES_ACTIVE
};
#endif
