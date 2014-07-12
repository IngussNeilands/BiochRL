#include "stdafx.h"
// #include "libtcod.hpp"
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <cstdlib>
#include <unordered_map>
#include <assert.h>

#include <SDL_mixer.h>
#include <windows.h>

#include "input.h"
#include "game.h"
#include <actors\Person.h>
#include "map.h"
#include "utils.h"
#include "inventory.h"
#include "Representation.h"
#include "tile.h"	
#include "equipment.h"
#include "ui.h"
#include "item.h"
#include "debug_options.h"
#include "combat.h"
#include "attribute_container.h"
#include "attribute.h"
#include "enums\screens.h"
#include "spells.h"
#include "attr_effect.h"
#include "class.h"
#include <messages.h>
#include "custom_key.h"
#include <color_utils.h>
#include <thinker.h>

//build key mappings. Took DRY out behind the shed.
std::map<char, basic_cmds_t> Input::char_main_keymap                  = Input::build_char_main_keymap(); 
std::map<char, classes_active_t> Input::char_class_select_keymap      = Input::build_char_class_select_keymap(); 
std::map<char, directions_t> Input::char_movement_keymap              = Input::build_char_movement_keymap();  
std::map<char, generic_menu_active_t> Input::char_generic_menu_keymap = Input::build_char_generic_menu_keymap(); 
std::map<char, inventory_items_active_t> Input::char_inventory_keymap = Input::build_char_inventory_keymap(); 
std::map<char, spells_active_t> Input::char_spell_select_keymap       = Input::build_char_spell_select_keymap(); 

std::map<int, basic_cmds_t> Input::spec_main_keymap                   = Input::build_spec_main_keymap(); 
std::map<int, classes_active_t> Input::spec_class_select_keymap       = Input::build_spec_class_select_keymap(); 
std::map<int, directions_t> Input::spec_movement_keymap               = Input::build_spec_movement_keymap();  
std::map<int, generic_menu_active_t> Input::spec_generic_menu_keymap  = Input::build_spec_generic_menu_keymap(); 
std::map<int, inventory_items_active_t> Input::spec_inventory_keymap  = Input::build_spec_inventory_keymap(); 
std::map<int, spells_active_t> Input::spec_spell_select_keymap        = Input::build_spec_spell_select_keymap(); 

char Input::generic_index = 'a';

std::vector<std::string> make_basic_cmds_char()
{
    std::vector<std::string> vec;
    vec.push_back("Pickup an item"); vec.push_back("Equip an item off the floor");
    vec.push_back("Drop an item");
    vec.push_back("Open your inventory");
    vec.push_back("Cancel targetting mode");
    vec.push_back("Look around the map"); vec.push_back("Open or close a door");
    vec.push_back("Go down stairs"); vec.push_back("Go upstairs");
    vec.push_back("Open magic menu"); vec.push_back("Cast the currently selected spell");
    vec.push_back("Open your character sheet");
    vec.push_back("Open this help screen"); vec.push_back("Select your class");
    vec.push_back("Center the screen on the mouse");
    vec.push_back("Target Mode: Cycle forwards through the targets"); vec.push_back("Target Mode: Cycle backwards through the targets");
    vec.push_back("Toggle Music");
    vec.push_back("Toggle Sneaking");
    vec.push_back("Use spell or ability in slot 1");
    vec.push_back("Use spell or ability in slot 2");
    vec.push_back("Use spell or ability in slot 3");
    vec.push_back("Use spell or ability in slot 4");
    vec.push_back("Use spell or ability in slot 5");
    vec.push_back("NO_MATCHING_BASIC_CMD");

    assert(vec.size() == basic_cmds_t::NO_MATCHING_BASIC_CMD+1 && "Missing a help command for basic chars");

    return vec;

};
std::vector<std::string> Input::basic_cmds_char = make_basic_cmds_char();

std::vector<std::string> make_inventory_items_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Examine the item");
    vec.push_back("Equip the active item"); vec.push_back("Unequip the active item");
    vec.push_back("Drop the active item"); vec.push_back("Exit the menu");
    vec.push_back("Use the active item \(if its a corpse, eat it for food\)");
    vec.push_back("Sort inventory by equippable");
    vec.push_back("Sort inventory by usable");
    vec.push_back("Sort inventory by equipped");
    vec.push_back("Zap the wand, casting its spell");
    vec.push_back("Mark this item as one of your quick keys");
    vec.push_back("NO_MATCHING_ITEMS_ACTIVE");

    assert(vec.size() == NO_MATCHING_ITEMS_ACTIVE+1 && "Missing a help command for inventory chars");
    return vec;
};
std::vector<std::string> Input::inventory_items_active_char = make_inventory_items_active_char();

std::vector<std::string> make_generic_menu_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Exit the menu");
    vec.push_back("NO_MATCHING_GENERIC_MENU_ACTIVE");
    assert(vec.size() == NO_MATCHING_GENERIC_MENU_ACTIVE+1 && "Missing a help command for generic chars");
    return vec;
};
std::vector<std::string> Input::generic_menu_active_char = make_generic_menu_active_char();

std::vector<std::string> make_spells_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Examine the spell");
    vec.push_back("Equip the spell"); vec.push_back("Unequip the spell");
    vec.push_back("Drop the spell"); vec.push_back("Exit the menu");
    vec.push_back("Cast the spell");
    vec.push_back("Mark this spell as one of your quick keys");
    vec.push_back("NO_MATCHING_SPELLS_ACTIVE");
    assert(vec.size() == NO_MATCHING_SPELLS_ACTIVE+1 && "Missing a help command for spell chars");
    return vec;
};
std::vector<std::string> Input::spells_active_char = make_spells_active_char();

std::vector<std::string> make_classes_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Examine Class");
    vec.push_back("Choose class"); vec.push_back("UnequipClass");
    vec.push_back("DropClass"); vec.push_back("EscapeMenuClass");
    vec.push_back("CastClass");
    vec.push_back("NO_MATCHING_CLASSES_ACTIVE");
    assert(vec.size() == NO_MATCHING_CLASSES_ACTIVE+1 && "Missing a help command for class chars");
    return vec;
};
std::vector<std::string> Input::classes_active_char = make_classes_active_char();

std::map<int, basic_cmds_t> Input::build_spec_main_keymap()
{
    std::map<int, basic_cmds_t> spec_movemap; //Keypad, punctuation
    spec_movemap[TCODK_SPACE] = basic_cmds_t::CenterScreenToMouse;
    spec_movemap[TCODK_PAGEUP] = basic_cmds_t::NextTarget;
    spec_movemap[TCODK_PAGEDOWN] = basic_cmds_t::PrevTarget;
    spec_movemap[TCODK_DELETE] = basic_cmds_t::ToggleMusic;

    spec_movemap[TCODK_1] = basic_cmds_t::CustomKey1;
    spec_movemap[TCODK_2] = basic_cmds_t::CustomKey2;
    spec_movemap[TCODK_3] = basic_cmds_t::CustomKey3;
    spec_movemap[TCODK_4] = basic_cmds_t::CustomKey4;
    spec_movemap[TCODK_5] = basic_cmds_t::CustomKey5;
    return spec_movemap;
};//Keypad, punctuation

std::map<char, basic_cmds_t> Input::build_char_main_keymap()
{
    std::map<char, basic_cmds_t> char_movemap; //regular letters

    char_movemap['q'] = basic_cmds_t::CancelCast;
    char_movemap['i'] = basic_cmds_t::OpenInventory;
    char_movemap['c'] = basic_cmds_t::OpenCharacterSheet;
    char_movemap['s'] = basic_cmds_t::ToggleSneaking;
    char_movemap['?'] = basic_cmds_t::OpenHelp;
    char_movemap[','] = basic_cmds_t::Pickup;
    char_movemap['.'] = basic_cmds_t::EquipFromFloor;
    char_movemap['o'] = basic_cmds_t::ActivateDoor;
    char_movemap['>'] = basic_cmds_t::DownStairs;
    char_movemap['<'] = basic_cmds_t::UpStairs;
    char_movemap['m'] = basic_cmds_t::OpenMagic;
    char_movemap['k'] = basic_cmds_t::ConfirmCast;
    char_movemap['p'] = basic_cmds_t::OpenClassSelect;

    return char_movemap;
};//regular letters

basic_cmds_t  Input::basic_cmd_pressed(TCOD_key_t key)
{
    return Input::find_key(key, Input::char_main_keymap, Input::spec_main_keymap, basic_cmds_t::NO_MATCHING_BASIC_CMD);
};

std::map<int, spells_active_t>  Input::build_spec_spell_select_keymap()
{
    std::map<int, spells_active_t> spec_spellactivemap; //Keypad, punctuation
    // 
    spec_spellactivemap[TCODK_ESCAPE] = spells_active_t::EscapeMenuSpell;
    return spec_spellactivemap;
}; //Keypad, punctuation

std::map<char, spells_active_t> Input::build_char_spell_select_keymap()
{
    std::map<char, spells_active_t> char_spellactivemap; //regular letters
    // 
    //     spec_spellactivemap['\''] = spells_active_t::N;

    char_spellactivemap['x'] = spells_active_t::ExamineSpell;
    // char_spellactivemap['d'] = spells_active_t::DropSpell;
    char_spellactivemap['c'] = spells_active_t::CastSpell;
    // char_spellactivemap['e'] = spells_active_t::EquipSpell;
    // char_spellactivemap['y'] = spells_active_t::UnequipSpell;
    char_spellactivemap['q'] = spells_active_t::EscapeMenuSpell;
    char_spellactivemap['m'] = spells_active_t::MarkSpell;
    return char_spellactivemap;
}; //regular letters

spells_active_t Input::spells_active_pressed(TCOD_key_t key)
{

    return Input::find_key(key, Input::char_spell_select_keymap,
            Input::spec_spell_select_keymap, spells_active_t::NO_MATCHING_SPELLS_ACTIVE);
};
std::map<int, classes_active_t>  Input::build_spec_class_select_keymap()
{
    std::map<int, classes_active_t> spec_classactivemap; //Keypad, punctuation
    // 
    spec_classactivemap[TCODK_ESCAPE] = classes_active_t::EscapeMenuClass;

    return spec_classactivemap;

}; //Keypad, punctuation
std::map<char, classes_active_t>  Input::build_char_class_select_keymap()
{
    std::map<char, classes_active_t> char_classactivemap; //regular letters
    // 
    //     spec_classactivemap['\''] = classes_active_t::N;

    char_classactivemap['x'] = classes_active_t::ExamineClass;
    // char_classactivemap['d'] = classes_active_t::Dropclass;
    char_classactivemap['c'] = classes_active_t::CastClass;
    char_classactivemap['e'] = classes_active_t::ChangeToClass;
    // char_classactivemap['y'] = classes_active_t::Unequipclass;
    char_classactivemap['q'] = classes_active_t::EscapeMenuClass;

    return char_classactivemap;
}; //regular letters

classes_active_t Input::classes_active_pressed(TCOD_key_t key)
{
    return Input::find_key(key, Input::char_class_select_keymap,
            Input::spec_class_select_keymap, classes_active_t::NO_MATCHING_CLASSES_ACTIVE);
};

    template<typename T1, typename T2, typename T3>
T1 Input::find_key(TCOD_key_t key, T2 char_active_map, T3 spec_active_map, T1 error_choice)
{
    if (key.vk == TCODK_CHAR) 
    {
        auto it = char_active_map.find(key.c);
        if(it == char_active_map.end())
        {
            return error_choice;
        }
        return it->second;
    }
    else
    {
        auto it = spec_active_map.find(key.vk);
        if(it == spec_active_map.end())
        {
            return error_choice;
        }
        return it->second;
    }

};

std::map<int, inventory_items_active_t>  Input::build_spec_inventory_keymap()
{

    std::map<int, inventory_items_active_t> spec_invitemactivemap; 
    spec_invitemactivemap[TCODK_ESCAPE] = inventory_items_active_t::EscapeMenuItem;
    return spec_invitemactivemap;
}; 

std::map<char, inventory_items_active_t> Input::build_char_inventory_keymap()
{

    std::map<char, inventory_items_active_t> char_invitemactivemap; 

    char_invitemactivemap['x'] = inventory_items_active_t::ExamineItem;
    char_invitemactivemap['d'] = inventory_items_active_t::DropItem;
    char_invitemactivemap['u'] = inventory_items_active_t::UseItem;
    char_invitemactivemap['e'] = inventory_items_active_t::EquipItem;
    char_invitemactivemap['y'] = inventory_items_active_t::UnequipItem;
    char_invitemactivemap['E'] = inventory_items_active_t::SortByEquippedItem;
    char_invitemactivemap['Y'] = inventory_items_active_t::SortByEquippableItem;
    char_invitemactivemap['U'] = inventory_items_active_t::SortByUsableItem;
    char_invitemactivemap['z'] = inventory_items_active_t::ZapItem;
    char_invitemactivemap['q'] = inventory_items_active_t::EscapeMenuItem;
    char_invitemactivemap['m'] = inventory_items_active_t::MarkItem;

    return char_invitemactivemap;
}; 

inventory_items_active_t Input::inventory_items_active_pressed(TCOD_key_t key)
{

    return find_key(key, Input::char_inventory_keymap,
            Input::spec_inventory_keymap, inventory_items_active_t::NO_MATCHING_ITEMS_ACTIVE);
};



std::map<int, directions_t>  Input::build_spec_movement_keymap()
{

    std::map<int, directions_t> spec_movemap; //Keypad, punctuation

    spec_movemap[TCODK_KP7] = directions_t::NW;
    spec_movemap[TCODK_KP8] = directions_t::N;
    spec_movemap[TCODK_UP] = directions_t::N;
    spec_movemap[TCODK_KP9] = directions_t::NE;
    spec_movemap[TCODK_KP6] = directions_t::E;
    spec_movemap[TCODK_RIGHT] = directions_t::E;
    spec_movemap[TCODK_KP3] = directions_t::SE;
    spec_movemap[TCODK_KP2] = directions_t::S;
    spec_movemap[TCODK_DOWN] = directions_t::S;
    spec_movemap[TCODK_KP1] = directions_t::SW;
    spec_movemap[TCODK_KP4] = directions_t::W;
    spec_movemap[TCODK_LEFT] = directions_t::W;

    spec_movemap[TCODK_KP5] = directions_t::X;

    return spec_movemap;
};  //Keypad, punctuation

std::map<char, directions_t> Input::build_char_movement_keymap()
{

    std::map<char, directions_t> char_movemap; //regular letters


    // char_movemap['n'] = directions_t::N;
    // char_movemap['e'] = directions_t::E;
    // char_movemap['s'] = directions_t::S;
    // char_movemap['w'] = directions_t::W;
    return char_movemap;
};  

directions_t Input::direction_pressed(TCOD_key_t key)
{

    return find_key(key, Input::char_movement_keymap, Input::spec_movement_keymap,
            directions_t::NO_MATCHING_DIRECTION);
};

bool Input::process_basic_keys(TCOD_key_t request)
{
    basic_cmds_t basic_cmd = Input::basic_cmd_pressed(request);

    //ignore key releases
    if (request.pressed == false)
    {
        return false;
    };

    if (basic_cmd == basic_cmds_t::Pickup)
    {
        std::cout << "PICKUP THIS IS A STICKUP" << std::endl;

        //check if items are on the floor
        if (Game::player->my_tile->check_for_items())
        {
            std::cout << "items on the floor, you'll be picking up";
            std::cout << "the last item you picked up now" << std::endl;
            //TODO:open ui for item pickup to choose which item
            Item* item = Game::player->my_tile->inventory->items->back();
            Game::player->pick_up_item(item);
            // player->equipment->chest->AddToSlot(item);
            new Message(Ui::msg_handler_main, ITEM_MSG, "Picked up the item.");

            return true;

        };
    }

    else if ( basic_cmd == basic_cmds_t::EquipFromFloor )
    {
        if (Game::player->my_tile->check_for_items())
        {
            Item* item = Game::player->my_tile->inventory->items->back();
            //drop item in the same slot
            if (item->equippable)
            {
                //drop currently equipped item
                Slot* equipped_slot = Game::player->equipment->get_slots_for_type(item->slot_type);
                Item* equipped_item = equipped_slot->GetEquippedItem();
                if (equipped_item != NULL)
                {
                    Game::player->equipment->unequip_item(equipped_item);
                    Game::player->inventory->drop_item(equipped_item);
                }

                //pickup and equip new one
                Game::player->pick_up_item(item);
                Game::player->equipment->equip_item(item);
                item->equip(Game::player);
                new Message(Ui::msg_handler_main, ITEM_MSG, "Equipped an item straight off the floor, impressive.");

                return true;
            }
            else if (item->usable)
            {
                Game::player->pick_up_item(item);
                item->use(Game::player);
                new Message(Ui::msg_handler_main, ITEM_MSG, "Used an item straight off the floor, gross.");
            }

        };
    }

    else if ( basic_cmd == basic_cmds_t::OpenInventory )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::InventoryScreenType;
        //FIXME this lets you save a turn if you want to cancel casting
        Ui::reset_generic();
    }

    else if ( basic_cmd == basic_cmds_t::CancelCast && Ui::is_targetting)
    {
        Ui::is_targetting = false;
    }

    else if ( basic_cmd == basic_cmds_t::CenterScreenToMouse )
    {
        Game::center_camera_on_player();
        // Game::center_camera_on(Game::get_mouse_tile());
    }

    else if ( basic_cmd == basic_cmds_t::OpenMagic )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::SpellSelectScreenType;
    }

    else if ( basic_cmd == basic_cmds_t::OpenClassSelect )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::ClassSelectScreenType;
    }

    else if ( basic_cmd == basic_cmds_t::OpenCharacterSheet )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::CharacterSheetScreenType;
    }

    else if ( basic_cmd == basic_cmds_t::OpenHelp )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::HelpScreenType;
    }

    else if ( basic_cmd == basic_cmds_t::ActivateDoor )
    {
        //determine the door to open
        //get the tile of the direction the player is facing
        Tile* door_tile;
        int* direction = Game::player->get_direction_heading();
        int x, y;

        x = Game::player->x+direction[0];
        y = Game::player->y+direction[1];

        door_tile = Game::current_map->getTileAt(x, y);

        //get the door that's on it
        if (door_tile->type_id == TileTypes::DoorTileTypeType) 
        {
            std::cout << "there's a door here, its gon get open" << std::endl;
            // door_tile->map->l_map->setProperties(x, y, true, true);
            door_tile->ToggleDoor();
        }
        else 
        {
            std::cout << "there's NO DAMN door here" << std::endl;
        };
        //open the door
    }
    else if ( basic_cmd == basic_cmds_t::CustomKey1 )
    {
        if (Game::custom_key1->element != NULL)
        {
            Game::custom_key1->activate();
        }
        else 
        {
            printf("Custom Key 1 is not bound to an item or spell\n");
        }
    }
    else if ( basic_cmd == basic_cmds_t::CustomKey2 )
    {
        if (Game::custom_key2->element != NULL)
        {
            Game::custom_key2->activate();
        }
        else 
        {
            printf("Custom Key 2 is not bound to an item or spell\n");
        }

    }
    else if ( basic_cmd == basic_cmds_t::CustomKey3 )
    {
        if (Game::custom_key3->element != NULL)
        {
            Game::custom_key3->activate();
        }
        else 
        {
            printf("Custom Key 3 is not bound to an item or spell\n");
        }
    }
    else if ( basic_cmd == basic_cmds_t::CustomKey4 )
    {
        if (Game::custom_key4->element != NULL)
        {
            Game::custom_key4->activate();
        }
        else 
        {
            printf("Custom Key 4 is not bound to an item or spell\n");
        }
    }
    else if ( basic_cmd == basic_cmds_t::CustomKey5 )
    {
        if (Game::custom_key5->element != NULL)
        {
            Game::custom_key5->activate();
        }
        else 
        {
            printf("Custom Key 5 is not bound to an item or spell\n");
        }
    }

    else if ( basic_cmd == basic_cmds_t::UpStairs )
    {
        Tile* stair_tile = Game::player->my_tile;
        if (stair_tile->type_id == TileTypes::StairsUpTileTypeType)
        {
            //std::cout << "there's a stair down here, its gon go down" << std::endl;
            new Message(Ui::msg_handler_main, NOTYPE_MSG,"There's a stair up here, its gon go down to the next floor.");
            ((StairsUpTileType*)stair_tile)->GoUp();
        }
        else
        {
            //std::cout << "There's no stair here" << std::endl;
            new Message(Ui::msg_handler_main, NOTYPE_MSG,"There's no stair here.");
        };
    }

    else if ( basic_cmd == basic_cmds_t::DownStairs )
    {
        Tile* stair_tile = Game::player->my_tile;
        if (stair_tile->type_id == TileTypes::StairsDownTileTypeType)
        {
            //std::cout << "there's a stair down here, its gon go down" << std::endl;
            new Message(Ui::msg_handler_main, NOTYPE_MSG,"There's a stair down here, its gon go down to the next floor.");
            ((StairsDownTileType*)stair_tile)->GoDown();
        }
        else
        {
            //std::cout << "There's no stair here" << std::endl;
            new Message(Ui::msg_handler_main, NOTYPE_MSG,"There's no stair here.");
        };
    }
    else if ( basic_cmd == basic_cmds_t::NextTarget )
    {
        Game::targetting_index++;

        if (Ui::is_targetting) 
        {
            int max_range = ((Spell*)Ui::chosen_generic)->max_range;
            actor_vec_t targets;
            Tile* mouse_tile = Game::get_mouse_tile();
            for (int i = 0; i < max_range; i++) {
                auto pts = points_around_circle(i, Game::player->my_tile->tile_x, Game::player->my_tile->tile_y);
                for (auto it = pts.begin(); it != pts.end(); it++)
                {
                    int x = it->at(0);
                    int y = it->at(1);
                    Tile* tile = Game::current_map->getTileAt(x, y);
                    if ( tile->is_occupied() )
                    {
                        targets.push_back(tile->occupant);
                    }

                }
            };

            // printf("%d\n", Game::targetting_index);
            if ( Game::targetting_index < targets.size())
            {

                Ui::targetted_tile = targets.at(Game::targetting_index)->my_tile;
            }
            else
            {
                if (targets.size() != 0)
                {
                    Ui::targetted_tile = targets.back()->my_tile;
                }
                else
                {
                    Ui::targetted_tile == Game::player->my_tile;
                }
            }
        }
    }
    else if ( basic_cmd == basic_cmds_t::PrevTarget )
    {
        Game::targetting_index--;
        if (Ui::is_targetting) 
        {

            int max_range = ((Spell*)Ui::chosen_generic)->max_range;
            actor_vec_t targets;
            Tile* mouse_tile = Game::get_mouse_tile();
            for (int i = 0; i < max_range; i++) {
                auto pts = points_around_circle(i, Game::player->my_tile->tile_x, Game::player->my_tile->tile_y);
                for (auto it = pts.begin(); it != pts.end(); it++)
                {
                    int x = it->at(0);
                    int y = it->at(1);
                    Tile* tile = Game::current_map->getTileAt(x, y);
                    if ( tile->is_occupied())
                    {
                        if (Game::player->is_actor_in_sight(tile->occupant))
                        {
                            targets.push_back(tile->occupant);
                        };
                    }

                }
            };
            // printf("%d\n", Game::targetting_index);
            if ( Game::targetting_index < targets.size())
            {

                Ui::targetted_tile = targets.at(Game::targetting_index)->my_tile;
            }
            else
            {
                if (targets.size() != 0)
                {
                    Ui::targetted_tile = targets.back()->my_tile;
                }
                else
                {
                    Ui::targetted_tile == Game::player->my_tile;
                }
            }
        }
    }
    else if ( basic_cmd == basic_cmds_t::ToggleSneaking)
    {
        bool incr_turn = Input::toggle_sneaking();
        return incr_turn;
    }
    else if ( basic_cmd == basic_cmds_t::ToggleMusic)
    {
        if (Mix_PausedMusic())
        {
            Mix_ResumeMusic();
        }
        else
        {
            Mix_PauseMusic();
        };
    }
    else if ( basic_cmd == basic_cmds_t::ConfirmCast && Ui::is_targetting )
    {
        return Input::user_cast_spell();
    }
    else 
    {
        printf("no matching key right now\n");
    };
    return false;
};

bool Input::toggle_sneaking()
{

    Game::player->is_sneaking = !Game::player->is_sneaking;
    if (Game::player->is_sneaking)
    {
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::lightBlue, "You are now sneaking."));
        Game::player->representation->setBGColor(TCODColor::darkGrey, true, false, true);

        if (Game::player->actor_class->type == StalkerClassType) //can cloak immediately
        {
            //mark all enemies that can't see you unaware now, since you slipped away
            actor_vec_t* actors_in_sight = Game::player->actors_in_sight;
            for (actor_vec_t::iterator it = Game::current_map->enemies.begin(); it != Game::current_map->enemies.end(); it++)
            {
                Actor* actor = (*it);
                bool player_can_see = std::find(actors_in_sight->begin(), actors_in_sight->end(), actor) == actors_in_sight->end();
                if (actor->thinker != NULL && player_can_see)
                {
                    actor->thinker->set_aware(false);
                }
            }
        };

    }
    else
    {
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::lightBlue, "You are no longer sneaking."));
        Game::player->representation->setBGColor(Game::game_console->getDefaultBackground(), true, false, true);
    };
    return true;
};

bool Input::user_cast_spell()
{
    Spell* spell = (Spell*)Ui::chosen_generic;
    if (spell == NULL)
    {
        std::cout << "no spell chosen" << std::endl;
        return false;
    }

    assert(spell->master != NULL && "Spell's master is null, it should be done on spell creation");
    Tile* targetted_tile = Ui::targetted_tile;
    int mana_cost = spell->mana_cost;
    int spell_range = spell->max_range;

    int distance = get_euclidean_distance(Game::player->x, Game::player->y, targetted_tile->tile_x, targetted_tile->tile_y);
    if (spell->is_valid_target(targetted_tile))
    {
        if (spell->is_in_range(distance))
        {
            if (spell->has_enough_mana())
            {
                spell->cast(targetted_tile);
                new Message(Ui::msg_handler_main, NOTYPE_MSG, colfg(spell->get_spell_color(), "Invoked %s!"), spell->name.c_str());
                return true;
            }
        }
    }

};


std::map<int, generic_menu_active_t>  Input::build_spec_generic_menu_keymap()
{

    std::map<int, generic_menu_active_t> spec_genmenactivemap; //Keypad, punctuation
    // 
    // spec_genmenactivemap[TCODK_ESCAPE] = generic_menu_active_t::EscapeGenericMenu;
    return spec_genmenactivemap;
}; //Keypad, punctuation

std::map<char, generic_menu_active_t> Input::build_char_generic_menu_keymap()
{

    std::map<char, generic_menu_active_t> char_genmenactivemap; //regular letters
    char_genmenactivemap['q'] = generic_menu_active_t::EscapeGenericMenu;
    return char_genmenactivemap;
}; //regular letters

generic_menu_active_t Input::generic_menu_active_pressed(TCOD_key_t key)
{

    return Input::find_key(key, Input::char_generic_menu_keymap,
            Input::spec_generic_menu_keymap, generic_menu_active_t::NO_MATCHING_GENERIC_MENU_ACTIVE);
};

bool Input::process_generic_menu_keys(TCOD_key_t request)
{
    generic_menu_active_t action = Input::generic_menu_active_pressed(request);

    if( action == generic_menu_active_t::EscapeGenericMenu )
    {
        // Ui::generic_active = false;
        // Ui::chosen_generic = false;
        //Ui::reset_generic();
        std::cout << "Back to the game." << std::endl;
        // Ui::reset_generic();
        // Input::generic_index = 'a';
        Game::current_state = GameStates::GameplayState;
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Escape back to regular inventory mode.");
        return true;
    }

    return false;
};

bool sort_by_equipped(Item* lhs, Item* rhs) { 
    Equipment* equipment = Game::player->equipment;
    if (equipment->is_item_equipped(lhs) && equipment->is_item_equipped(rhs))
    {
        return false;
    }
    else if (equipment->is_item_equipped(lhs) && !equipment->is_item_equipped(rhs))
    {
        return true;
    }
    else if (!equipment->is_item_equipped(lhs) && equipment->is_item_equipped(rhs))
    {
        return false;
    }
    else
    {
        return false;
    };
};

bool sort_by_equippable(Item* lhs, Item* rhs) { 
    if (lhs->equippable && rhs->equippable)
    {
        return false;
    }
    else if (lhs->equippable && !rhs->equippable)
    {
        return true;
    }
    else if (!lhs->equippable && rhs->equippable)
    {
        return false;
    }
    else
    {
        return false;
    };
};

bool sort_by_usable(Item* lhs, Item* rhs) { 
    if (lhs->usable && rhs->usable)
    {
        return false;
    }
    else if (lhs->usable && !rhs->usable)
    {
        return true;
    }
    else if (!lhs->usable && rhs->usable)
    {
        return false;
    }
    else
    {
        return false;
    };
};
bool Input::process_inventory_keys(TCOD_key_t request)
{
    inventory_items_active_t action = inventory_items_active_pressed(request);

    if( action == inventory_items_active_t::ExamineItem )
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "%s", ((Item*)Ui::chosen_generic)->description);
        std::cout << ((Item*)Ui::chosen_generic)->name << ". " << ((Item*)Ui::chosen_generic)->description << std::endl;
        return true;
    }
    else if( action == inventory_items_active_t::DropItem )
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "DROP ITEM.");
        Item* item = static_cast<Item*>(Ui::chosen_generic);
        Ui::reset_generic();

        Game::player->inventory->drop_item(item);
        return true;
    }

    else if( action == inventory_items_active_t::UseItem )
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Using item.");
        Item* item = ((Item*)Ui::chosen_generic);
        item->use(Game::player);
        if (item->uses == 0)
        {
            Ui::reset_generic();
        };
        return true;
    }

    else if( action == inventory_items_active_t::SortByEquippedItem )
    {
        std::sort(Game::player->inventory->items->begin(), Game::player->inventory->items->end(), sort_by_equipped);
        return false;
    }

    else if( action == inventory_items_active_t::SortByEquippableItem )
    {
        std::sort(Game::player->inventory->items->begin(), Game::player->inventory->items->end(), sort_by_equippable);
        return false;
    }

    else if( action == inventory_items_active_t::SortByUsableItem )
    {
        std::sort(Game::player->inventory->items->begin(), Game::player->inventory->items->end(), sort_by_usable);
        return false;
    }

    else if( action == inventory_items_active_t::ZapItem )
    {
        if (((Item*)Ui::chosen_generic)->spell_effect != NULL)
        {
            Ui::is_targetting = true;
            Ui::targetted_tile = Game::player->my_tile;
            Game::current_state = GameStates::GameplayState;
            Ui::chosen_generic = ((Item*)Ui::chosen_generic)->spell_effect;
        }
        else
        {
            std::cout << "Item has no zappable effect." << std::endl;
        };
        return false;
    }

    else if ( action == inventory_items_active_t::EquipItem )
    {
        ((Item*)Ui::chosen_generic)->equip(Game::player);
        Game::player->equipment->equip_item(((Item*)Ui::chosen_generic));
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Equipping item.");
        return true;
    }

    else if ( action == inventory_items_active_t::MarkItem )
    {
        if (((Item*)Ui::chosen_generic)->spell_effect == NULL) { printf("no spell attached\n"); return false; };
        //take int input
        std::cout << "Choose a single digit between 1 and 5 to use as a quick key" << std::endl << ">>>" ;
        minimize_game();
        int input;
        std::cin >> input;
        maximize_game();
        //convert to ints
        if (input > 5) { input = 5;}
        else if (input < 0 ) { input = 0; };

        if (input == 1) 
        {
            Game::custom_key1->assign_item((Item*)Ui::chosen_generic);
        }
        else if (input == 2) 
        {
            Game::custom_key2->assign_item((Item*)Ui::chosen_generic);
        }
        else if (input == 3) 
        {
            Game::custom_key3->assign_item((Item*)Ui::chosen_generic);
        }
        else if (input == 4) 
        {
            Game::custom_key4->assign_item((Item*)Ui::chosen_generic);
        }
        else if (input == 5) 
        {
            Game::custom_key5->assign_item((Item*)Ui::chosen_generic);
        }

        //assign item to proper custom_key
        return true;
    }

    else if( action == inventory_items_active_t::UnequipItem )
    {
        Item* item = static_cast<Item*>(Ui::chosen_generic);
        item->unequip(Game::player);
        Game::player->equipment->unequip_item(item);

        ckey_vec_t_it it = Game::custom_keys->begin();
        for (it; it != Game::custom_keys->end(); it++)
        {
            if ((*it)->element == item)
            {
                (*it)->reset_state();
            };
        }
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Unequipping item.");
        return true;
    }

    else if( action == inventory_items_active_t::EscapeMenuItem )
    {
        Ui::generic_active = false;
        Ui::chosen_generic = false;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Escape back to regular inventory mode.");
        return true;
    }

    return false;
};

bool Input::process_classes_keys(TCOD_key_t request)
{
    classes_active_t action = classes_active_pressed(request);

    if( action == classes_active_t::ExamineClass )
    {
        IClass* iclass = (IClass*)Ui::chosen_generic;
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "EXAMINE CLASS.");
        std::cout << iclass->name << " : " << iclass->description << std::endl;
        return true;
    }
    else if( action == classes_active_t::DropClass )
    {/*
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "DROP ITEM.");
        Class* spell = Ui::chosen_item;
        Ui::chosen_spell = NULL;
        Ui::spell_active = false;

        player->inventory->drop_spell(item);
        return true;
        */}

    else if( action == classes_active_t::CastClass )
    {
        // Ui::toggle_targetting();
        // Game::current_state = GameStates::GameplayState;
        // std::cout << Ui::chosen_spell->name << std::endl;
        // Game::current_screen = Screens::Game
        //new Message(Ui::msg_handler_main, NOTYPE_MSG, "Using spell.");
        //Ui::chosen_spell->use(Game::player);
        //return true;
    }

    else if( action == classes_active_t::ChangeToClass )
    {
        Game::player->actor_class = NULL;
        Game::player->actor_class = (IClass*)Ui::chosen_generic;
        Game::player->actor_class->master = Game::player;
        Game::player->representation->setFGColor(Game::player->actor_class->fg_color, true, false, true);
        std::cout << "Class changed to " << ((IClass*)Ui::chosen_generic)->name << std::endl;
        // Ui::chosen_spell->equip(Game::player);
        // Game::player->equipment->equip_spell(Ui::chosen_item);
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Equipping spell.");
        Ui::reset_generic();
        Input::generic_index = 'a';
        Game::current_state = GameStates::GameplayState;
        // return true;
    }

    else if( action == classes_active_t::UnequipClass )
    {
        // Ui::chosen_spell->unequip(Game::player);
        // Game::player->equipment->unequip_spell(Ui::chosen_item);
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Unequipping spell.");
        // return true;
    }

    else if( action == classes_active_t::EscapeMenuClass )
    {
        std::cout << "escape" << std::endl;
        // if (  Ui::generic_active == false )
        // {
        //     Game::current_state = GameStates::GameplayState;
        // };
        Ui::generic_active = false;
        Ui::chosen_generic = false;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Escape back to regular inventory mode.");
        std::cout << "Escape back to regular classmenu mode." << std::endl;
        return true;
    }

    return false;
};

bool Input::process_spells_keys(TCOD_key_t request)
{
    spells_active_t action = spells_active_pressed(request);

    if( action == spells_active_t::ExamineSpell )
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "EXAMINE SPELL.");
        std::cout << ((Spell*)Ui::chosen_generic)->name << std::endl;
        return true;
    }
    else if( action == spells_active_t::DropSpell )
    {/*
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "DROP ITEM.");
        Spell* spell = Ui::chosen_item;
        Ui::chosen_spell = NULL;
        Ui::spell_active = false;

        player->inventory->drop_spell(item);
        return true;
        */}

    else if( action == spells_active_t::CastSpell )
    {
        Ui::is_targetting = true;
        Ui::targetted_tile = Game::player->my_tile;
        Game::current_state = GameStates::GameplayState;
        std::cout << ((Spell*)Ui::chosen_generic)->name << std::endl;
    }

    else if( action == spells_active_t::EquipSpell )
    {
    }

    else if( action == spells_active_t::UnequipSpell )
    {
    }
    else if( action == spells_active_t::MarkSpell )
    {
        {
            //take int input
            std::cout << "Choose a single digit between 1 and 5 to use as a quick key" << std::endl << ">>>" ;
            minimize_game();
            int input;
            std::cin >> input;
            maximize_game();
            //convert to ints
            if (input > 5) { input = 5;}
            else if (input < 0 ) { input = 0; };

            if (input == 1) 
            {
                Game::custom_key1->assign_spell((Spell*)Ui::chosen_generic);
            }
            else if (input == 2) 
            {
                Game::custom_key2->assign_spell((Spell*)Ui::chosen_generic);
            }
            else if (input == 3) 
            {
                Game::custom_key3->assign_spell((Spell*)Ui::chosen_generic);
            }
            else if (input == 4) 
            {
                Game::custom_key4->assign_spell((Spell*)Ui::chosen_generic);
            }
            else if (input == 5) 
            {
                Game::custom_key5->assign_spell((Spell*)Ui::chosen_generic);
            }

            //assign item to proper custom_key
            return true;
        }
    }

    else if( action == spells_active_t::EscapeMenuSpell )
    {
        Ui::generic_active = false;
        Ui::chosen_generic = false;
        Ui::is_targetting = false;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Escape back to regular spellmenu mode.");
        std::cout <<  "Escape back to regular spellmenu mode." << std::endl;
        return true;
    }

    return false;
};

void move_camera(int dir_x, int dir_y)
{
    //if player moves within 5 spaces of the edge of the map, scroll the camera 
    //so that they're no longer within 5 spaces

    // camera checks player position relative to camera
    // then checks camera position relative to world
    // if player 5< spaces from cam limits move camera so that player has 5>
    // spaces assuming camera has that much space
    int cam_x2 = Game::camera_x + Game::camera_w;
    int cam_y2 = Game::camera_y + Game::camera_h;

    //relative player pos
    int plr_x;
    int plr_y;
    if (!Ui::is_targetting)
    {
        plr_x = Game::player->x - Game::camera_x; 
        plr_y = Game::player->y - Game::camera_y;
    }
    else
    {
        plr_x = Ui::targetted_tile->tile_x;
        plr_y = Ui::targetted_tile->tile_y;
    };

    int border_threshold = 10;
    //std::cout << "cam w" << Game::camera_w << std::endl;

    int change = 1;
    if (plr_x <= border_threshold || plr_x >= (Game::camera_w -border_threshold))
    {
        //adjust camera horizontally
        if (plr_x <= border_threshold) 
        {
            if (Game::camera_x - change >= (0))
            {
                Game::camera_x -= 1;
                // std::cout << "x left" << std::endl;
            }
            else
            {
                // std::cout << "x NOT left" << std::endl;
            }
        }
        else 
        {
            if (cam_x2 + change <= (Game::map_width))
            {
                Game::camera_x += 1;
                // std::cout << "x right" << std::endl;
            }
            else
            {
                // std::cout << "x NOT right" << std::endl;
            }
        };
    };
    if (plr_y <= border_threshold || plr_y >= (Game::camera_h -border_threshold))
    {
        //adjust camera vertically
        if (plr_y <= border_threshold) 
        {
            if (Game::camera_y - change >= (0 ))
            {
                Game::camera_y -= 1;
                // std::cout << "y up" << std::endl;
            }
            else
            {
                // std::cout << "y NOT up" << std::endl;
            }
        }
        else 
        {
            if (cam_y2 + change <= (Game::map_height))
            {
                Game::camera_y += 1;
                // std::cout << "y down" << std::endl;
            }
            else
            {
                // std::cout << "y NOT down" << std::endl;
            }
        };
    };
};

bool Input::move_target(int x, int y)
{
    //std::cout << "move_target" << std::endl;
    Ui::targetted_tile = Ui::targetted_tile->getTileAtRelative(x, y);
    move_camera(x, y);
    return false;
};

bool Input::move_player_or_target(int x, int y)
{
    if (! Ui::is_targetting)
    {
        if(Game::current_map->attackMovePlayer(Game::player, x, y) )
        { 
            move_camera(x, y);
            return true;
        }
    }
    else 
    {
        Input::move_target(x, y);
    };

    return false;
};

//returns whether or not the player has moved and should increment the turn
bool Input::process_movement_keys(TCOD_key_t request)
{
    Map *world = Game::current_map;
    bool has_moved = false;

    int orig_plr_pos_x, orig_plr_pos_y;
    orig_plr_pos_x = Game::player->x;
    orig_plr_pos_y = Game::player->y;

    Game::player->is_moving_up = false;
    Game::player->is_moving_down = false;
    Game::player->is_moving_left = false;
    Game::player->is_moving_right = false;
    //reset player facing directions
    if (request.pressed == false)
    {
        Game::player->resetIsMoving();
        return false;
    };

    directions_t direction = direction_pressed(request);
    if( direction == directions_t::N )
    {
        Game::player->is_moving_up = true;
        has_moved = Input::move_player_or_target(0, -1);
    }
    else if( direction == directions_t::NE )
    {
        Game::player->is_moving_up = true;
        Game::player->is_moving_right = true;
        has_moved = Input::move_player_or_target(1, -1);
    }
    else if( direction == directions_t::S )
    {
        Game::player->is_moving_down = true;
        has_moved = Input::move_player_or_target(0, 1);
    }
    else if( direction == directions_t::SE)
    {
        Game::player->is_moving_down = true;
        Game::player->is_moving_right = true;
        has_moved = Input::move_player_or_target(1, 1);
        // if(Game::current_map->attackMovePlayer(Game::player, 1, 1) )
        // {
        //     move_camera(1, 1);
        //     return true;
        // }
    }
    else if( direction == directions_t::E)
    {
        Game::player->is_moving_right = true;
        has_moved = Input::move_player_or_target(1, 0);
        // if(Game::current_map->attackMovePlayer(Game::player, 1, 0) )
        // {
        //     move_camera(1, 0);

        //     return true;
        // }
    }
    else if( direction == directions_t::SW)
    {
        Game::player->is_moving_left = true;
        Game::player->is_moving_down = true;
        has_moved = Input::move_player_or_target(-1, 1);

    }
    else if( direction == directions_t::NW)
    {
        Game::player->is_moving_left = true;
        Game::player->is_moving_up = true;
        has_moved = Input::move_player_or_target(-1, -1);
        // if(Game::current_map->attackMovePlayer(Game::player, -1, -1) )
        // { 
        //     move_camera(-1, -1);

        //     return true;
        // }
    }
    else if( direction == directions_t::W)
    {
        Game::player->is_moving_left = true;
        has_moved = Input::move_player_or_target(-1, 0);
        // if(Game::current_map->attackMovePlayer(Game::player, -1, 0) )
        // { 
        //     move_camera(-1, 0);

        //     return true;
        // }
    }
    else if( direction == directions_t::X)
    {
        if (Ui::is_targetting)
        {
            return Input::user_cast_spell();
        }
        else
        {
            new Message(Ui::msg_handler_main, NOTYPE_MSG, "Waiting in place.");
            return true;
        }
    };

    if (has_moved) return true;


    //if the player has moved or attacked this update, increment the turn
    //counter
    if ((orig_plr_pos_x != Game::player->x || orig_plr_pos_y != Game::player->y || (Game::player->has_attacked && !Ui::is_targetting)))
    {
        return true;
    }
    else
    {
        return false;
    }

};

bool Input::is_key_basic_command(TCOD_key_t request)
{
    return Input::basic_cmd_pressed(request) != basic_cmds_t::NO_MATCHING_BASIC_CMD;
};

bool Input::is_key_move_command(TCOD_key_t request)
{
    return Input::direction_pressed(request) != directions_t::NO_MATCHING_DIRECTION;
};

bool Input::is_key_inventory_command(TCOD_key_t request)
{
    return Input::inventory_items_active_pressed(request) != inventory_items_active_t::NO_MATCHING_ITEMS_ACTIVE;
};

bool Input::is_key_generic_menu_command(TCOD_key_t request)
{
    return Input::generic_menu_active_pressed(request) != generic_menu_active_t::NO_MATCHING_GENERIC_MENU_ACTIVE;
};

bool Input::is_key_spell_command(TCOD_key_t request)
{
    return Input::spells_active_pressed(request) != spells_active_t::NO_MATCHING_SPELLS_ACTIVE;
};

bool Input::is_key_class_command(TCOD_key_t request)
{
    return Input::classes_active_pressed(request) != classes_active_t::NO_MATCHING_CLASSES_ACTIVE;
};

void Input::process_mouse_inv_event()
{
    if (Game::mouse_evt.lbutton_pressed)
    {

    };
};

bool Input::process_mouse_event(TCOD_mouse_t request)
{

    if (request.dx > Ui::mouse_move_threshold || request.dy > Ui::mouse_move_threshold)
    {
        //std::cout << "Mouse MOVE" << std::endl;
        Ui::tick_mouse_moved = Game::tick_count;
    }
    //set the foreground color to red for the tile the mouse is on
    // Tile* moused_tile = Game::current_map->getTileAt(Game::mouse_evt.cx+Game::camera_x, Game::mouse_evt.cy+Game::camera_y);
    // moused_tile->tile->representation->temp_bg_color = &(TCODColor)(TCODColor::red); //this only works because we get a new red every turn

    if (request.lbutton_pressed && !Ui::is_targetting)
    {
        Game::center_camera_on(Game::get_mouse_tile());
    }

    if (request.lbutton_pressed && Ui::is_targetting)
    {
        return Input::user_cast_spell();
    }

    return false;
};

bool Input::process_debug_event(TCOD_key_t request)
{
    if (request.vk == TCODK_F2)
    {
        exit(1);
    };

    if (request.vk == TCODK_F3)
    {
        Game::debug_opts->all_vision = !Game::debug_opts->all_vision;
    }

    if (request.vk == TCODK_F4)
    {
        auto map = Game::build_world(Game::current_map->depth+1);
        Game::current_map = map;
    }

    if (request.vk == TCODK_F5)
    {
        //this'll redraw the entire screen incase shit goes black
        TCODConsole::root->setDirty(0, 0, 1000, 1000);
    }

    if (request.vk == TCODK_F6)
    {
        //this'll let you see the type and index of *new* messages
        Game::debug_opts->show_msg_debug = !Game::debug_opts->show_msg_debug;
    }

    if (request.vk == TCODK_F7)
    {
        //this'll level the player up
        if (request.shift == true)
        {
            Game::player->combat->LevelUp(10);
        }
        else if (request.shift == false) 
        {
            Game::player->combat->LevelUp(1);
        };
    }

    if (request.vk == TCODK_F8)
    {
        //this'll redraw the entire screen incase shit goes black
        TCODConsole::root->setDirty(0, 0, 1000, 1000);
    }

    if (request.vk == TCODK_F11)
    {
        //toggle fullscreen
        TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
    }
    if (request.vk == TCODK_F12)
    {
        //this'll redraw the entire screen incase shit goes black
        // TCODConsole::root->setDirty(0, 0, 1000, 1000);
        Game::player->die();
    }

    return false;
};

bool Input::process_key_event(TCOD_key_t request)
{

    Ui::tick_key_pressed = Game::tick_count;
    //std::cout << "key event" << std::endl;
    //determine if movement command
    bool incr_turn = false;

    switch(Game::current_state)
    {
        case GameStates::GameplayState: 

            if(is_key_move_command(request))
            {
                incr_turn = Input::process_movement_keys(request);
                if (incr_turn && direction_pressed(request) != directions_t::X)
                {
                    //stop the targetting so that user has to retry
                    Ui::is_targetting = false;
                    Ui::reset_generic();
                }
            }

            else if (is_key_basic_command(request))
            {
                incr_turn = Input::process_basic_keys(request);
            }

            else if(request.vk == TCODK_ESCAPE && request.pressed == 1)
            {

                std::cout << "Are you sure you want to quit?" << std::endl << ">>> " ;
                minimize_game();
                std::string resp;
                std::cin >> resp;
                if (resp.at(0) == 'y')
                {
                    std::cout << "Goodbye now" << std::endl;
                    exit(1);
                };
                maximize_game();
            }
            else
            {
                std::cout << std::endl << "command not found: " << char_to_str(request.c) << std::endl;
                std::cout << "nswe or numpad to move, i to open inventory, ESC to quit, o to open doors" << std::endl;
                std::cout << "c to open character sheet, m and k to cast spells, ? for help, > to go down" << std::endl;
            }

            break;

        case GameStates::MenuState:
            //handle arrow keys and enter, then go to screen specfic handlers
            if (Game::current_screen == Screens::SpellSelectScreenType)
            {
                std::vector<Spell*>* spells = Game::player->spells;
                Input::select_generic(request, spells, is_key_spell_command, Input::process_spells_keys);

            }
            else if (Game::current_screen == Screens::ClassSelectScreenType)
            {
                Ui::is_targetting = false;
                std::vector<IClass*>* classes = Actor::actor_class_choices;
                Input::select_generic(request, classes, is_key_class_command, Input::process_classes_keys);
            }

            else if (Game::current_screen == Screens::InventoryScreenType)
            {
                Ui::is_targetting = false;
                std::vector<Item*>* items = Game::player->inventory->items;
                Input::select_generic(request, items, is_key_inventory_command, Input::process_inventory_keys);
            }
            else 
            {
                Ui::is_targetting = false;
                std::vector<void*>* _ = new std::vector<void*>;
                Input::select_generic(request, _, is_key_generic_menu_command, Input::process_generic_menu_keys);
                delete _;
            }
            break;
    }

    return incr_turn;

};

generic_keypair_t Input::build_keypairs(int limit, int offset)
{
    generic_keypair_t keymap;
    char key = 'a';
    // for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
    for (int index = offset; index < limit; index++)
    {
        keymap.insert(generic_keypair(key, index));
        if (key == 'z') break;
        key++;
    }

    return keymap;
};


    template<class T>
void Input::match_key(char letter, generic_keypair_t generic_map, std::vector<T*>* generic_vector, bool allow_activate)
{
    generic_keypair_t::iterator it = generic_map.find(letter);
    if (it != generic_map.end())
    {
        Input::generic_index = letter;
        if ((T*)Ui::chosen_generic == generic_vector->at(it->second))
        {
            if (allow_activate)
            {
                Ui::generic_active = true;
            };
            Input::generic_index = letter;
        }
        else
        {
            Ui::generic_active = false;
        };
        Ui::chosen_generic = generic_vector->at(it->second);
    };
};

    template<class T>
void Input::select_generic(TCOD_key_t request, std::vector<T*>* elements, bool (*active_func)(TCOD_key_t), bool (*process_func)(TCOD_key_t))
{
    int size = elements->size();
    bool successful_action = true;

    generic_keypair_t key_map = Input::build_keypairs(size, Ui::offset);
    generic_keypair_t::iterator it = key_map.find(request.c);

    if (Ui::generic_active == false)
    {
        if (request.c == 'q' && request.pressed == 1)// && Ui::generic_active == false)
        {
            std::cout << "Back to the game." << std::endl;
            Ui::reset_generic();
            Input::generic_index = 'a';
            Game::current_state = GameStates::GameplayState;
        }
        else if (request.vk == TCODK_UP && request.pressed == 1)
        {
            Input::generic_index--;
            if (Input::generic_index < 'a' ) 
            {

                //go to prev page unless its the first page
                if (Ui::offset == 0)
                {
                    Input::generic_index = 'a';
                }
                else
                {
                    Ui::page_num--;
                    Ui::offset = Ui::per_page*Ui::page_num;
                    Input::generic_index = 'a' + Ui::per_page-1;
                    key_map = Input::build_keypairs(size, Ui::offset);
                };
                //  set g_i to last character on page
            }
            Input::match_key<T>(Input::generic_index, key_map, elements, false);
        }
        else if (request.vk == TCODK_DOWN && request.pressed == 1)
        {
            //check to see if pressing down takes you too far down the list
            std::vector<T*>::iterator it = elements->begin()+Ui::offset;
            int selection_index = Input::generic_index-97;
            it+=selection_index;
            if (it+1 != elements->end())
            {
                Input::generic_index++;
            };

            if (Input::generic_index >= 'a' + Ui::per_page)
            {
                //go to next page unless its the last page
                if (Ui::offset+Ui::per_page > elements->size())
                {
                    Input::generic_index--;
                }
                else
                {
                    Ui::page_num++;
                    Ui::offset = Ui::per_page*Ui::page_num;
                    Input::generic_index = 'a';
                    key_map = Input::build_keypairs(size, Ui::offset);
                };
            }

            Input::match_key<T>(Input::generic_index, key_map, elements, false);
        }

        else if (request.vk == TCODK_ENTER && request.pressed == 1)
        {
            Input::match_key<T>(Input::generic_index, key_map, elements, true);
        }
        else if ( (request.vk == TCODK_PAGEDOWN || request.c == '+') && request.pressed == 1) 
        {
            Ui::page_num++;
            Ui::offset = Ui::per_page*Ui::page_num;
        }
        else if ( (request.vk == TCODK_HOME) && request.pressed == 1) 
        {
            Ui::page_num = 0;
            Ui::offset = Ui::per_page*Ui::page_num;
        }
        else if ( (request.vk == TCODK_PAGEUP || request.c == '-' )&& request.pressed == 1) 
        {
            if (Ui::offset > 0)
            {
                Ui::page_num--;
                Ui::offset = Ui::per_page*Ui::page_num;
            };
        }
        else 
        {
            //choose generic from letter to generic map
            Input::match_key<T>(request.c, key_map, elements, true);
        };
    }
    else // generic_active is true
    {
        if (active_func(request))
        {
            successful_action = process_func(request);
        }
        else 
        {
            auto vk_map = Input::get_tcodkey_to_string_map();
            if (request.c == 0 && vk_map.find(request.vk) != vk_map.end())
            {
                std::cout << std::endl << "command not found: " << vk_map.at(request.vk) << std::endl;
            }
            else
            {
                std::cout << std::endl << "command not found: " << char_to_str(request.c) << std::endl;
            };

            std::cout << "q to return to gameplay, a b c to choose the first, second, third element etc." << std::endl;
            std::cout << "press again to select. once it's activated, press u to use items" << std::endl;
            std::cout << "e to equip classes or items, y to unequip items, d to drop items" << std::endl;
            std::cout << "c to choose the spell to cast, z to zap the item" << std::endl;
            std::cout << "x to examine the spell, item or class" << std::endl;
        }
    }

};

std::map<int, std::string> Input::get_tcodkey_to_string_map()
{
    std::map<int, std::string> keymap;
    keymap[TCODK_ESCAPE] = "Escape";
    keymap[TCODK_BACKSPACE] = "Backspace";
    keymap[TCODK_TAB] = "Tab";
    keymap[TCODK_ENTER] = "Enter";
    keymap[TCODK_SHIFT] = "Shift";
    keymap[TCODK_CONTROL] = "Control";
    keymap[TCODK_ALT] = "Alt";
    keymap[TCODK_PAUSE] = "Pause";
    keymap[TCODK_CAPSLOCK] = "CapsLock";
    keymap[TCODK_PAGEUP] = "PageUp";
    keymap[TCODK_PAGEDOWN] = "PageDown";
    keymap[TCODK_END] = "End";
    keymap[TCODK_HOME] = "Home";
    keymap[TCODK_UP] = "Up";
    keymap[TCODK_LEFT] = "Left";
    keymap[TCODK_RIGHT] = "Right";
    keymap[TCODK_DOWN] = "Down";
    keymap[TCODK_PRINTSCREEN] = "Printscreen";
    keymap[TCODK_INSERT] = "Insert";
    keymap[TCODK_DELETE] = "Delete";
    keymap[TCODK_LWIN] = "Lwin";
    keymap[TCODK_RWIN] = "Rwin";
    keymap[TCODK_APPS] = "Apps";
    keymap[TCODK_KPADD] = "KeypadAdd";
    keymap[TCODK_KPSUB] = "KeypadSubtract";
    keymap[TCODK_KPDIV] = "KeypadDivide";
    keymap[TCODK_KPMUL] = "KeypadMultiply";
    keymap[TCODK_KPDEC] = "KeypadDecimal";
    keymap[TCODK_KPENTER] = "KeypadEnter";
    keymap[TCODK_F1] = "F1";
    keymap[TCODK_F2] = "F2";
    keymap[TCODK_F3] = "F3";
    keymap[TCODK_F4] = "F4";
    keymap[TCODK_F5] = "F5";
    keymap[TCODK_F6] = "F6";
    keymap[TCODK_F7] = "F7";
    keymap[TCODK_F8] = "F8";
    keymap[TCODK_F9] = "F9";
    keymap[TCODK_F10] = "F10";
    keymap[TCODK_F11] = "F11";
    keymap[TCODK_F12] = "F12";
    keymap[TCODK_NUMLOCK] = "Numlock";
    keymap[TCODK_SCROLLLOCK] = "Scrolllock";
    keymap[TCODK_SPACE] = "Space";
    keymap[TCODK_0] = "Zero";
    keymap[TCODK_1] = "One";
    keymap[TCODK_2] = "Two";
    keymap[TCODK_3] = "Three";
    keymap[TCODK_4] = "Four";
    keymap[TCODK_5] = "Five";
    keymap[TCODK_6] = "Six";
    keymap[TCODK_7] = "Seven";
    keymap[TCODK_8] = "Eight";
    keymap[TCODK_9] = "Nine";
    keymap[TCODK_KP0] = "KeypadZero";
    keymap[TCODK_KP1] = "KeypadOne";
    keymap[TCODK_KP2] = "KeypadTwo";
    keymap[TCODK_KP3] = "KeypadThree";
    keymap[TCODK_KP4] = "KeypadFour";
    keymap[TCODK_KP5] = "KeypadFive";
    keymap[TCODK_KP6] = "KeypadSix";
    keymap[TCODK_KP7] = "KeypadSeven";
    keymap[TCODK_KP8] = "KeypadEight";
    keymap[TCODK_KP9] = "KeypadNine";
    return keymap;
};
