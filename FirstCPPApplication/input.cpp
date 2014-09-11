#include "stdafx.h"
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
#include <thinker.h>
#include "draw_utils.h"
#include "helpbox.h"
#include <menu_item.h>
#include <background_info.h>

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
    vec.push_back("Show bound custom keys");
    vec.push_back("Center the screen on the mouse");
    vec.push_back("When targetting: Cycle forwards through the targets"); vec.push_back("When targetting: Cycle backwards through the targets");
    vec.push_back("Toggle Music");
    vec.push_back("Toggle Sneaking");
    vec.push_back("Use spell or ability in slot 1");
    vec.push_back("Use spell or ability in slot 2");
    vec.push_back("Use spell or ability in slot 3");
    vec.push_back("Use spell or ability in slot 4");
    vec.push_back("Use spell or ability in slot 5");
    vec.push_back("NO_MATCHING_BASIC_CMD");

    assert(vec.size() == basic_cmds_t::LAST_BASIC_ACTIVE+1 && "Missing a help command for basic chars");

    return vec;

};
std::vector<std::string> Input::basic_cmds_char = make_basic_cmds_char();

std::vector<std::string> make_inventory_items_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Examine the item");
    vec.push_back("Equip the active item"); vec.push_back("Unequip the active item");
    vec.push_back("Drop the active item"); vec.push_back("Exit the menu");
    vec.push_back("Use the active item (if its a corpse, eat it for food)");
    vec.push_back("Sort inventory by equippable");
    vec.push_back("Sort inventory by usable");
    vec.push_back("Sort inventory by equipped");
    vec.push_back("Zap the wand, casting its spell");
    vec.push_back("Mark this item as one of your quick keys");
    vec.push_back("NO_MATCHING_ITEMS_ACTIVE");

    assert(vec.size() == LAST_ITEMS_ACTIVE+1 && "Missing a help command for inventory chars");
    return vec;
};
std::vector<std::string> Input::inventory_items_active_char = make_inventory_items_active_char();

std::vector<std::string> make_generic_menu_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Exit the menu");
    vec.push_back("Confirm the menu item");
    vec.push_back("Examine the menu item");
    vec.push_back("NO_MATCHING_GENERIC_MENU_ACTIVE");
    assert(vec.size() == LAST_GENERIC_MENU_ACTIVE+1 && "Missing a help command for generic chars");
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
    assert(vec.size() == LAST_SPELLS_ACTIVE+1 && "Missing a help command for spell chars");
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
    assert(vec.size() == LAST_CLASSES_ACTIVE+1 && "Missing a help command for class chars");
    return vec;
};
std::vector<std::string> Input::classes_active_char = make_classes_active_char();

std::map<int, basic_cmds_t> Input::build_spec_main_keymap()
{
    std::map<int, basic_cmds_t> spec_movemap; 
    spec_movemap[TCODK_SPACE] = basic_cmds_t::CenterScreenToMouse;

    spec_movemap[TCODK_PAGEUP] = basic_cmds_t::NextTarget;
    spec_movemap[TCODK_PAGEDOWN] = basic_cmds_t::PrevTarget;
    spec_movemap[TCODK_DELETE] = basic_cmds_t::ToggleMusic;
    spec_movemap[TCODK_ENTER] = basic_cmds_t::ConfirmCast;

    spec_movemap[TCODK_1] = basic_cmds_t::CustomKey1;
    spec_movemap[TCODK_2] = basic_cmds_t::CustomKey2;
    spec_movemap[TCODK_3] = basic_cmds_t::CustomKey3;
    spec_movemap[TCODK_4] = basic_cmds_t::CustomKey4;
    spec_movemap[TCODK_5] = basic_cmds_t::CustomKey5;
    return spec_movemap;
};

std::map<char, basic_cmds_t> Input::build_char_main_keymap()
{
    std::map<char, basic_cmds_t> char_movemap; 

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
    char_movemap['`'] = basic_cmds_t::ListCustomKeys;

    return char_movemap;
};

basic_cmds_t  Input::basic_cmd_pressed(TCOD_key_t key)
{
    return Input::find_key(key, Input::char_main_keymap, Input::spec_main_keymap, basic_cmds_t::NO_MATCHING_BASIC_CMD);
};

std::map<int, spells_active_t>  Input::build_spec_spell_select_keymap()
{
    std::map<int, spells_active_t> spec_spellactivemap; 
    // 
    spec_spellactivemap[TCODK_ESCAPE] = spells_active_t::EscapeMenuSpell;
    spec_spellactivemap[TCODK_ENTER] = spells_active_t::CastSpell;
    return spec_spellactivemap;
}; 

std::map<char, spells_active_t> Input::build_char_spell_select_keymap()
{
    std::map<char, spells_active_t> char_spellactivemap; 
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
}; 

spells_active_t Input::spells_active_pressed(TCOD_key_t key)
{

    return Input::find_key(key, Input::char_spell_select_keymap,
            Input::spec_spell_select_keymap, spells_active_t::NO_MATCHING_SPELLS_ACTIVE);
};
std::map<int, classes_active_t>  Input::build_spec_class_select_keymap()
{
    std::map<int, classes_active_t> spec_classactivemap; 

    spec_classactivemap[TCODK_ESCAPE] = classes_active_t::EscapeMenuClass;
    spec_classactivemap[TCODK_ENTER] = classes_active_t::ChangeToClass;

    return spec_classactivemap;

}; 
std::map<char, classes_active_t>  Input::build_char_class_select_keymap()
{
    std::map<char, classes_active_t> char_classactivemap; 

    char_classactivemap['x'] = classes_active_t::ExamineClass;
    char_classactivemap['c'] = classes_active_t::CastClass;
    char_classactivemap['e'] = classes_active_t::ChangeToClass;
    char_classactivemap['q'] = classes_active_t::EscapeMenuClass;

    return char_classactivemap;
}; 

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
    spec_invitemactivemap[TCODK_ENTER] = inventory_items_active_t::EquipItem;
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

    std::map<int, directions_t> spec_movemap; 

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
};  

std::map<char, directions_t> Input::build_char_movement_keymap()
{

    std::map<char, directions_t> char_movemap; 


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

    else if ( basic_cmd == basic_cmds_t::ListCustomKeys )
    {
        std::vector<std::string> details_msgs = std::vector<std::string>();
        details_msgs.push_back("Custom Keys");
        details_msgs.push_back(" ");
        for (auto it = Game::custom_keys->begin(); it!=Game::custom_keys->end();it++)
        {
            CustomKey* ck = *it;
            if (ck->is_bound())
                // if (true)
            {
                std::stringstream ss;
                ss << ck->index << " -> " << ck->get_element_name();
                details_msgs.push_back(ss.str());
            }
        }
        details_msgs.push_back(" ");
        details_msgs.push_back("Hit N/Q to continue");
        DialogHelpBox* details_dialog = new DialogHelpBox(details_msgs, Game::game_console);
        details_dialog->return_screen = Game::current_screen;
        details_dialog->y = 5;
        details_dialog->x = 5;
        Ui::alerts.push_back(details_dialog);
        Game::current_screen = Screens::AlertScreenType;
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
            ((StairsUpTileType*)stair_tile->tiletype_obj)->GoUp();
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
            StairsDownTileType* down_stairs_tiletype = static_cast<StairsDownTileType*>(stair_tile->tiletype_obj);
            if (down_stairs_tiletype->tile_obj == NULL)
            {
                down_stairs_tiletype->tile_obj = stair_tile;
            };
            down_stairs_tiletype->GoDown();
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
                    Ui::targetted_tile = Game::player->my_tile;
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
                    Ui::targetted_tile = Game::player->my_tile;
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
        Game::fov_radius-= 3;
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
        Game::fov_radius+= 3;
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
        else
        {
            new Message(Ui::msg_handler_main, NOTYPE_MSG, "Out of range. Max is %i, you're at %i.", spell->max_range, distance);
        };
    }

    return false;

};


std::map<int, generic_menu_active_t>  Input::build_spec_generic_menu_keymap()
{

    std::map<int, generic_menu_active_t> spec_genmenactivemap; 
    spec_genmenactivemap[TCODK_ENTER] = generic_menu_active_t::ActivateGenericMenuItem;
    return spec_genmenactivemap;
}; 

std::map<char, generic_menu_active_t> Input::build_char_generic_menu_keymap()
{

    std::map<char, generic_menu_active_t> char_genmenactivemap; 
    char_genmenactivemap['q'] = generic_menu_active_t::EscapeGenericMenu;
    char_genmenactivemap['a'] = generic_menu_active_t::ActivateGenericMenuItem;
    char_genmenactivemap['x'] = generic_menu_active_t::ExamineGenericMenuItem;
    char_genmenactivemap['?'] = generic_menu_active_t::ExamineGenericMenuItem;
    return char_genmenactivemap;
}; 

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
        Ui::generic_active = false;
        Ui::chosen_generic = NULL;
        return true;
    }
    else if (action == generic_menu_active_t::ActivateGenericMenuItem)
    {
        MenuItem* menu_item = static_cast<MenuItem*>(Ui::chosen_generic);
		if (menu_item->info->type > 50) //hack to sorta validate the menu_item
		{
			printf("log: mit\n");
		}
		else
		{
        menu_item->activate();
		};
    }
    else if (action == generic_menu_active_t::ExamineGenericMenuItem)
    {
        MenuItem* menu_item = static_cast<MenuItem*>(Ui::chosen_generic);
        menu_item->examine();
    };

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
            if ((*it)->is_item)
            {
                Item* element = static_cast<Item*>((*it)->element);
                if (element == item)
                {
                    (*it)->reset_state();
                };
            }
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


void close_all(DialogHelpBox* dialog)
{
    Game::current_screen = dialog->return_screen;
    Ui::clear_alerts();
};

bool Input::process_classes_keys(TCOD_key_t request)
{
    classes_active_t action = classes_active_pressed(request);

    if( action == classes_active_t::ExamineClass )
    {
        IClass* iclass = (IClass*)Ui::chosen_generic;
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "EXAMINE CLASS.");
        std::cout << iclass->name << " : " << iclass->description << std::endl;

        //create examine dialog
        std::vector<std::string> left_msgs = iclass->starting_attrs->VerboseVector();
        left_msgs.insert(left_msgs.begin(), "Examining "+iclass->name);
        left_msgs.insert(left_msgs.begin()+1, "Starting Attributes");
        left_msgs.push_back(" ");
        left_msgs.push_back("Hit N/Q to continue");
        DialogHelpBox* left_dialog = new DialogHelpBox(left_msgs, NULL, &close_all, TCODConsole::root);
        left_dialog->return_screen = Game::current_screen;
        left_dialog->y = 5;
        left_dialog->x = 5;
        left_dialog->resize(left_dialog->width+10, left_dialog->height);
        Ui::alerts.push_back(left_dialog);

        //create examine dialog
        std::vector<std::string> right_msgs = std::vector<std::string>();
        right_msgs.insert(right_msgs.begin(), "Description");
        right_msgs.push_back(iclass->description);
        right_msgs.push_back("");
        right_msgs.push_back("Spell Progression");
        for (auto it = iclass->spell_map->begin(); it!= iclass->spell_map->end(); it++)
        {
            std::pair<int, Spell*> pair = *it;
            int at_level = pair.first;
            Spell* spell = pair.second;
            std::stringstream ss;
            ss << "L" << at_level << " " << colfg(spell->get_spell_color(), spell->name);
            right_msgs.push_back(ss.str());

        }
        right_msgs.push_back("");
        right_msgs.push_back("On level up");
        std::stringstream ss;
        ss << "HP:  " << iclass->health_on_lvl;
        right_msgs.push_back(ss.str());
        ss.str("");
        ss << "MAN: " << iclass->mana_on_lvl;
        right_msgs.push_back(ss.str());
        ss.str("");
        ss << "ARM: " << iclass->armor_on_lvl;
        right_msgs.push_back(ss.str());
        ss.str("");
        ss << "DMG: " << iclass->damage_on_lvl;
        right_msgs.push_back(ss.str());
        ss.str("");
        right_msgs.push_back("");
        right_msgs.push_back("Hit N/Q to continue");
        DialogHelpBox* right_dialog = new DialogHelpBox(right_msgs, NULL, &close_all, TCODConsole::root);
        right_dialog->return_screen = Game::current_screen;
        right_dialog->x = 45;
        right_dialog->y = 5;
        right_dialog->resize(right_dialog->width+10, right_dialog->height-1);
        Ui::alerts.push_back(right_dialog);

        Game::current_screen = Screens::AlertScreenType;
        return true;
    }

    else if( action == classes_active_t::ChangeToClass )
    {
        Game::player->actor_class = NULL;
        Game::player->actor_class = (IClass*)Ui::chosen_generic;
        Game::player->actor_class->master = Game::player;
        Game::player->representation->setFGColor(Game::player->actor_class->fg_color, true, false, true);
        std::cout << "Class changed to " << ((IClass*)Ui::chosen_generic)->name << std::endl;

        //modify starting stats if level 1
        if (Game::player->level == 1)
        {
            Game::player->actor_class->apply_starting_stats();
        };

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
    else if( action == spells_active_t::CastSpell )
    {
        Ui::is_targetting = true;
        Ui::targetted_tile = Game::player->my_tile;
        Game::current_state = GameStates::GameplayState;
        std::cout << ((Spell*)Ui::chosen_generic)->name << std::endl;
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


bool Input::move_target(int x, int y)
{
    //std::cout << "move_target" << std::endl;
    Ui::targetted_tile = Ui::targetted_tile->getTileAtRelative(x, y);
    Game::move_camera(x, y);
    return false;
};

bool Input::move_player_or_target(int x, int y)
{
    if (!Ui::is_targetting)
    {
        Game::player->is_defending = false;
        if (Game::current_map->attackMovePlayer(Game::player, x, y))
        { 
            Game::move_camera(x, y);
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
    }
    else if( direction == directions_t::E)
    {
        Game::player->is_moving_right = true;
        has_moved = Input::move_player_or_target(1, 0);
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
    }
    else if( direction == directions_t::W)
    {
        Game::player->is_moving_left = true;
        has_moved = Input::move_player_or_target(-1, 0);
    }
    else if( direction == directions_t::X)
    {
        if (Ui::is_targetting)
        {
            return Input::user_cast_spell();
        }
        else
        {
            Game::player->is_defending = true;
            new Message(Ui::msg_handler_main, NOTYPE_MSG, "Standing your ground.");
            return true;
        }
    };

    if (has_moved && direction != directions_t::X)
    {
        //stop the targetting so that user has to retry
        Ui::is_targetting = false;
        Ui::reset_generic();
    }


    //if the player has moved or attacked this update, increment the turn
    //counter
    if ((has_moved || (Game::player->has_attacked && !Ui::is_targetting)))
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
        Tile* mouse_tile = Game::get_mouse_tile();
        Game::center_camera_on(mouse_tile);
    }

    if (request.lbutton_pressed && Ui::is_targetting)
    {
        return Input::user_cast_spell();
    }

    return false;
};

void show_quick_help()
{
    TCODColor clr = TCODColor::lightGrey+TCODColor::yellow;

    std::vector<std::string> help_msgs;
    help_msgs.push_back("QUICK HELP");
    help_msgs.push_back(colfg(clr,"Arrow keys")+" or " +colfg(clr,"numpad")+" (diagonal movement is allowed) to move");
    help_msgs.push_back("Move into someone to attack them, but be careful!");
    help_msgs.push_back(colfg(clr,"i")+" for inventory, "+colfg(clr,"m")+" for magic");
    help_msgs.push_back(colfg(clr,"k KP5 LeftClick")+" to cast a spell once you're targetting");
    help_msgs.push_back(colfg(clr,"p")+" for class reselect (can at anytime, even after level up)");
    help_msgs.push_back(colfg(clr,"c")+" for character sheet");
    help_msgs.push_back(colfg(clr,"?")+" for more help");
    help_msgs.push_back("");
    help_msgs.push_back("N to continue");

    DialogHelpBox* help_dialog = new DialogHelpBox(help_msgs, Game::game_console);
    int x = Game::camera_w/2-5, y = Game::camera_h/2-15;
    help_dialog->x = x;
    help_dialog->y = y;

    help_dialog->resize(25, help_dialog->height+2);

    Ui::alerts.push_back(help_dialog);

    Game::current_screen = Screens::AlertScreenType;
};

bool Input::process_debug_event(TCOD_key_t request)
{
    if (request.vk == TCODK_F1)
    {
        //dont want to create more than one dialog
        if (Game::current_screen != Screens::AlertScreenType)
        {
            show_quick_help();
        };
    };

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
        Game::center_camera_on_player();
    }

    if (request.vk == TCODK_F5)
    {
        //this'll redraw the entire screen incase shit goes black
        // TCODConsole::root->setDirty(0, 0, 1000, 1000);
        std::vector<std::string> quit_msgs;
        std::stringstream ss;
        ss << "game queue ticks " << Game::queue_ticks;
        quit_msgs.push_back(ss.str());
        ss.str("");
        ss << "player target queue ticks " << Game::player->target_queue_tick;
        quit_msgs.push_back(ss.str());
        ss.str("");
        quit_msgs.push_back("Y/N");
        DialogHelpBox* quit_dialog = new DialogHelpBox(quit_msgs, Game::game_console);
        int x = Game::camera_w/2, y = Game::camera_h/2;
        quit_dialog->x = x;
        quit_dialog->y = y;
        Ui::alerts.push_back(quit_dialog);

        Game::current_screen = Screens::AlertScreenType;
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
            Game::player->combat->level_up(10);
        }
        else if (request.shift == false) 
        {
            Game::player->combat->level_up(1);
        };
    }

    if (request.vk == TCODK_F8)
    {
        //this'll redraw the entire screen incase shit goes black
        TCODConsole::root->setDirty(0, 0, 1000, 1000);
    }

    if (request.vk == TCODK_F9)
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::SimpleMenuScreenType;
        Game::current_background_type = background_types_t::GenderBackgroundType;
        Game::current_background_type = background_types_t::RaceBackgroundType;
        Game::current_background_type = background_types_t::HometownBackgroundType;
        // Game::current_screen = Screens::MainMenuScreenType;
    }

    if (request.vk == TCODK_F10)
    {
        Game::give_player_teleport(Game::player);
        Game::give_player_god_mode();
        Game::give_player_rich();
    }

    if (request.vk == TCODK_F11)
    {
        //toggle fullscreen
        TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
    }
    if (request.vk == TCODK_F12)
    {
        Game::player->die();
    }

    return false;
};

void quit_game_dialog(DialogHelpBox* dialog)
{
    Game::quit_game();
};

bool Input::print_bad_key(TCOD_key_t request)
{
    //returns should print message
    auto vk_map = Input::get_tcodkey_to_string_map();
    if (request.c == 0 && vk_map.find(request.vk) != vk_map.end())
    {
        if (request.vk == TCODK_SHIFT) { return false; };
        std::cout << std::endl << "command not found: " << vk_map.at(request.vk) << std::endl;
    }
    else
    {
        std::cout << std::endl << "command not found: " << char_to_str(request.c) << std::endl;
    };

    return true;
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

            if (Game::current_screen == Screens::AlertScreenType)
            {
                if (request.c == 'y')
                {
                    if (!Ui::alerts.empty())
                    {
                        DialogHelpBox* alert = Ui::alerts.back();
                        alert->accept();
                    }

                }
                else if (request.c == 'n' || request.c == 'q')
                {
                    if (!Ui::alerts.empty())
                    {
                        DialogHelpBox* alert = Ui::alerts.back();
                        alert->cancel();
                    }
                }
            }
            else 
            {
                if(is_key_move_command(request))
                {
                    incr_turn = Input::process_movement_keys(request);
                }

                else if (is_key_basic_command(request))
                {
                    incr_turn = Input::process_basic_keys(request);
                }

                else if(request.vk == TCODK_ESCAPE && request.pressed == 1)
                {

                    //create quit dialog
                    std::vector<std::string> quit_msgs;
                    quit_msgs.push_back("Quit?");
                    quit_msgs.push_back("Y/N");
                    DialogHelpBox* quit_dialog = new DialogHelpBox(quit_msgs, &quit_game_dialog, NULL, Game::game_console);
                    int x = Game::camera_w/2, y = Game::camera_h/2;
                    quit_dialog->x = x;
                    quit_dialog->y = y;
                    Ui::alerts.push_back(quit_dialog);

                    Game::current_screen = Screens::AlertScreenType;
                }
                else
                {
                    if (Input::print_bad_key(request))
                    {
                        std::cout << "nswe or numpad to move, i to open inventory, ESC to quit, o to open doors" << std::endl;
                        std::cout << "c to open character sheet, m and k to cast spells, ? for help, > to go down" << std::endl;
                    }
                }

            }
            break;

        case GameStates::MenuState:
            if (Game::current_screen == Screens::AlertScreenType)
            {
                if (request.c == 'y')
                {
                    if (!Ui::alerts.empty())
                    {
                        DialogHelpBox* alert = Ui::alerts.back();
                        alert->accept();
                    }

                }
                else if (request.c == 'n' || request.c == 'q')
                {
                    if (!Ui::alerts.empty())
                    {
                        DialogHelpBox* alert = Ui::alerts.back();
                        alert->cancel();
                    }
                }
            }
            else 
            {
                //handle arrow keys and enter, then go to screen specfic handlers
                if (Game::current_screen == Screens::SpellSelectScreenType)
                {
                    std::vector<Spell*>* spells = Game::player->spells;
                    std::vector<Spell*>* combined = new std::vector<Spell*>(*spells);
                    spell_vec_t attached_spells = Game::player->equipment->get_spells_attached();
                    combined->insert(combined->end(), attached_spells.begin(), attached_spells.end());
                    Input::select_generic(request, combined, is_key_spell_command, Input::process_spells_keys);
                    delete combined;

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

                else if (Game::current_screen == Screens::MainMenuScreenType)
                {
                    Ui::is_targetting = false;
                    str_vec_t* items = Game::menu_choices;
                    Input::select_generic(request, items, is_key_generic_menu_command, Input::process_generic_menu_keys);
                }
                else if (Game::current_screen == Screens::SimpleMenuScreenType)
                {
                    Ui::is_targetting = false;
                    menuitem_vec_t* items = (*Game::menuitem_choices)[Game::current_background_type];
                    Input::select_generic(request, items, is_key_generic_menu_command, Input::process_generic_menu_keys);
                }
                else 
                {
                    Ui::is_targetting = false;
                    std::vector<void*>* _ = new std::vector<void*>;
                    Input::select_generic(request, _, is_key_generic_menu_command, Input::process_generic_menu_keys);
                    delete _;
                }
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

        T* chosen_element = (T*)Ui::chosen_generic;
        // if (chosen_element == generic_vector->at(it->second))
        // {
        if (allow_activate)
        {
            Ui::generic_active = true;
        };
        Input::generic_index = letter;
        // }
        // else
        // {
        //     Ui::generic_active = false;
        // };
        Ui::chosen_generic = generic_vector->at(it->second);
    };
};

    template<class T>
void Input::select_generic(TCOD_key_t request, std::vector<T*>* elements, bool (*active_func)(TCOD_key_t), bool (*process_func)(TCOD_key_t))
{
    int size = elements->size();
    bool successful_action = true;

    generic_keypair_t key_map = Input::build_keypairs(size, Ui::offset);

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
            if (elements->empty()) { return; };
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
            if (elements->empty()) { return; };
            //check to see if pressing down takes you too far down the list
            std::vector<T*>::iterator it = elements->begin()+Ui::offset;
            int selection_index = Input::generic_index-97;
            it+=selection_index;
            if (selection_index+Ui::offset < elements->size() && it+1 != elements->end())
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
            if (Input::print_bad_key(request)) 
            {
                std::cout << "q to return to gameplay, a b c to choose the first, second, third element etc." << std::endl;
                std::cout << "press again to select. once it's activated, press u to use items" << std::endl;
                std::cout << "e to equip classes or items, y to unequip items, d to drop items" << std::endl;
                std::cout << "c to choose the spell to cast, z to zap the item" << std::endl;
                std::cout << "x to examine the spell, item or class" << std::endl;
            };
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
