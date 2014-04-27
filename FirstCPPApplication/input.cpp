#include "stdafx.h"
// #include "libtcod.hpp"
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <cstdlib>
#include <unordered_map>

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
#include "item_effect.h"
#include "class.h"

//build key mappings. Took DRY out behind the shed.
std::map<int, directions_t> Input::spec_movemap = Input::build_spec_movemap();  //Keypad, punctuation
std::map<char, directions_t>Input::char_movemap  = Input::build_char_movemap();  //Keypad, punctuation//regular letters
std::map<int, inventory_items_active_t> Input::spec_invitemactivemap = Input::build_spec_invitemactivemap(); //Keypad, punctuation
std::map<char, inventory_items_active_t>Input::char_invitemactivemap = Input::build_char_invitemactivemap(); //regular letters
std::map<int, generic_menu_active_t> Input::spec_genmenactivemap = Input::build_spec_genmenactivemap(); //Keypad, punctuation
std::map<char, generic_menu_active_t>Input::char_genmenactivemap = Input::build_char_genmenactivemap(); //regular letters
std::map<int, basic_cmds_t> Input::spec_active_map = Input::build_spec_active_map(); //Keypad, punctuation
std::map<char, basic_cmds_t>Input::char_active_map = Input::build_char_active_map(); //regular letters
std::map<int, spells_active_t> Input::spec_spellactivemap = Input::build_spec_spellactivemap(); //Keypad, punctuation
std::map<char, spells_active_t>Input::char_spellactivemap = Input::build_char_spellactivemap(); //regular letters
std::map<int, classes_active_t> Input::spec_classactivemap = Input::build_spec_classactivemap(); //Keypad, punctuation
std::map<char, classes_active_t> Input::char_classactivemap = Input::build_char_classactivemap(); //regular letters

std::vector<std::string> make_basic_cmds_char()
{
    std::vector<std::string> vec;
    vec.push_back("Pickup an item"); vec.push_back("Drop an item");
    vec.push_back("Open your inventory");
    vec.push_back("Look around the map"); vec.push_back("Open or close a door");
    vec.push_back("Go down stairs"); vec.push_back("Go upstairs");
    vec.push_back("Open magic menu"); vec.push_back("Cast the currently selected spell");
    vec.push_back("Open your character sheet");
    vec.push_back("Open this help screen"); vec.push_back("Select your class");
    vec.push_back("Center the screen on the mouse");
    vec.push_back("NO_MATCHING_BASIC_CMD");

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
    vec.push_back("NO_MATCHING_ITEMS_ACTIVE");
    return vec;
};
std::vector<std::string> Input::inventory_items_active_char = make_inventory_items_active_char();

std::vector<std::string> make_generic_menu_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("Exit the menu");
    vec.push_back("NO_MATCHING_GENERIC_MENU_ACTIVE");
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
    vec.push_back("NO_MATCHING_SPELLS_ACTIVE");
    return vec;
};
std::vector<std::string> Input::spells_active_char = make_spells_active_char();

std::vector<std::string> make_classes_active_char()
{
    std::vector<std::string> vec;
    vec.push_back("ExamineClass");
    vec.push_back("EquipClass"); vec.push_back("UnequipClass");
    vec.push_back("DropClass"); vec.push_back("EscapeMenuClass");
    vec.push_back("CastClass");
    vec.push_back("NO_MATCHING_CLASSES_ACTIVE");
    return vec;
};
std::vector<std::string> Input::classes_active_char = make_classes_active_char();

std::map<int, basic_cmds_t> Input::build_spec_active_map()
{
    std::map<int, basic_cmds_t> spec_movemap; //Keypad, punctuation
    spec_movemap[TCODK_SPACE] = basic_cmds_t::CenterScreenToMouse;
    return spec_movemap;
};//Keypad, punctuation

std::map<char, basic_cmds_t> Input::build_char_active_map()
{
    std::map<char, basic_cmds_t> char_movemap; //regular letters

    char_movemap['i'] = basic_cmds_t::OpenInventory;
    char_movemap['c'] = basic_cmds_t::OpenCharacterSheet;
    char_movemap['?'] = basic_cmds_t::OpenHelp;
    char_movemap[','] = basic_cmds_t::Pickup;
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
    return Input::find_key(key, Input::char_active_map, Input::spec_active_map, basic_cmds_t::NO_MATCHING_BASIC_CMD);
};

std::map<int, spells_active_t>  Input::build_spec_spellactivemap()
{
    std::map<int, spells_active_t> spec_spellactivemap; //Keypad, punctuation
    // 
    spec_spellactivemap[TCODK_ESCAPE] = spells_active_t::EscapeMenuSpell;
    return spec_spellactivemap;
}; //Keypad, punctuation
std::map<char, spells_active_t> Input::build_char_spellactivemap()
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
    return char_spellactivemap;
}; //regular letters

spells_active_t Input::spells_active_pressed(TCOD_key_t key)
{

    return Input::find_key(key, Input::char_spellactivemap,
            Input::spec_spellactivemap, spells_active_t::NO_MATCHING_SPELLS_ACTIVE);
};
std::map<int, classes_active_t>  Input::build_spec_classactivemap()
{
    std::map<int, classes_active_t> spec_classactivemap; //Keypad, punctuation
    // 
    spec_classactivemap[TCODK_ESCAPE] = classes_active_t::EscapeMenuClass;

    return spec_classactivemap;

}; //Keypad, punctuation
std::map<char, classes_active_t>  Input::build_char_classactivemap()
{
    std::map<char, classes_active_t> char_classactivemap; //regular letters
    // 
    //     spec_classactivemap['\''] = classes_active_t::N;

    char_classactivemap['x'] = classes_active_t::ExamineClass;
    // char_classactivemap['d'] = classes_active_t::Dropclass;
    char_classactivemap['c'] = classes_active_t::CastClass;
    char_classactivemap['e'] = classes_active_t::EquipClass;
    // char_classactivemap['y'] = classes_active_t::Unequipclass;
    char_classactivemap['q'] = classes_active_t::EscapeMenuClass;

    return char_classactivemap;
}; //regular letters

classes_active_t Input::classes_active_pressed(TCOD_key_t key)
{
    return Input::find_key(key, Input::char_classactivemap,
            Input::spec_classactivemap, classes_active_t::NO_MATCHING_CLASSES_ACTIVE);
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

std::map<int, inventory_items_active_t>  Input::build_spec_invitemactivemap()
{

    std::map<int, inventory_items_active_t> spec_invitemactivemap; //Keypad, punctuation
    // 
    spec_invitemactivemap[TCODK_ESCAPE] = inventory_items_active_t::EscapeMenuItem;
    return spec_invitemactivemap;
}; //Keypad, punctuation
std::map<char, inventory_items_active_t> Input::build_char_invitemactivemap()
{

    std::map<char, inventory_items_active_t> char_invitemactivemap; //regular letters
    // 

    char_invitemactivemap['x'] = inventory_items_active_t::ExamineItem;
    char_invitemactivemap['d'] = inventory_items_active_t::DropItem;
    char_invitemactivemap['u'] = inventory_items_active_t::UseItem;
    char_invitemactivemap['e'] = inventory_items_active_t::EquipItem;
    char_invitemactivemap['y'] = inventory_items_active_t::UnequipItem;
    char_invitemactivemap['q'] = inventory_items_active_t::EscapeMenuItem;

    return char_invitemactivemap;
}; //regular letters

inventory_items_active_t Input::inventory_items_active_pressed(TCOD_key_t key)
{

    return find_key(key, Input::char_invitemactivemap,
            Input::spec_invitemactivemap, inventory_items_active_t::NO_MATCHING_ITEMS_ACTIVE);
};



std::map<int, directions_t>  Input::build_spec_movemap()
{

    std::map<int, directions_t> spec_movemap; //Keypad, punctuation

    spec_movemap[TCODK_KP7] = directions_t::NW;
    spec_movemap[TCODK_KP8] = directions_t::N;
    spec_movemap[TCODK_KP9] = directions_t::NE;
    spec_movemap[TCODK_KP6] = directions_t::E;
    spec_movemap[TCODK_KP3] = directions_t::SE;
    spec_movemap[TCODK_KP2] = directions_t::S;
    spec_movemap[TCODK_KP1] = directions_t::SW;
    spec_movemap[TCODK_KP4] = directions_t::W;

    spec_movemap[TCODK_KP5] = directions_t::X;

    return spec_movemap;
};  //Keypad, punctuation

std::map<char, directions_t> Input::build_char_movemap()
{

    std::map<char, directions_t> char_movemap; //regular letters


    // char_movemap[TCODK_KP7] = directions_t::NW;
    char_movemap['n'] = directions_t::N;
    // char_movemap[TCODK_KP9] = directions_t::NE;
    char_movemap['e'] = directions_t::E;
    // char_movemap[TCODK_KP3] = directions_t::SE;
    char_movemap['s'] = directions_t::S;
    // char_movemap[TCODK_KP1] = directions_t::SW;
    char_movemap['w'] = directions_t::W;
    return char_movemap;
};  //Keypad, punctuation//regular letters

directions_t Input::direction_pressed(TCOD_key_t key)
{

    return find_key(key, Input::char_movemap, Input::spec_movemap,
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
            Game::player->pickUpItem(item);
            // player->equipment->chest->AddToSlot(item);

            return true;

        };
    }

    else if ( basic_cmd == basic_cmds_t::OpenInventory )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::InventoryScreen;
    }

    else if ( basic_cmd == basic_cmds_t::CenterScreenToMouse )
    {
        Game::center_camera_on(Game::get_mouse_tile());
    }

    else if ( basic_cmd == basic_cmds_t::OpenMagic )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::SpellSelectScreen;
    }

    else if ( basic_cmd == basic_cmds_t::OpenClassSelect )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::ClassSelectScreen;
    }

    else if ( basic_cmd == basic_cmds_t::OpenCharacterSheet )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::CharacterSheetScreen;
    }

    else if ( basic_cmd == basic_cmds_t::OpenHelp )
    {
        Game::current_state = GameStates::MenuState;
        Game::current_screen = Screens::HelpScreen;
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

        door_tile = Game::world->getTileAt(x, y);

        //get the door that's on it
        if (door_tile->type_id == TileTypes::DoorTileTypeType) //TODO: make an enum or something for type_ids
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

    // else if ( basic_cmd == basic_cmds_t::OpenMagic )
    // {
    //     Tile* stair_tile = Game::player->my_tile;
    //     if (Ui::toggle_targetting())
    //     {
    //         new Message(Ui::msg_handler_main, NOTYPE_MSG, "Select a target.");
    //     }
    //     else
    //     {
    //         new Message(Ui::msg_handler_main, NOTYPE_MSG, "Done targetting");
    //     }
    //     //start targetting mode
    // }

    else if ( basic_cmd == basic_cmds_t::ConfirmCast )
    {
        Spell* spell = (Spell*)Ui::chosen_generic;
        if (spell == NULL)
        {
            std::cout << "no spell chosen" << std::endl;
            return false;
        }
        Tile* mouse_tile = Ui::targetted_tile;
        // Tile* mouse_tile = Game::get_mouse_tile();
        // Spell* spell = Game::player->spells->back();
        int mana_cost = spell->mana_cost;
        int spell_range = spell->max_range;
        int spell_damage = spell->spell_effect->health_current_val;
        // int spell_damage = (-spell->attrs->health->current_val);

        int distance = get_euclidean_distance(Game::player->x, Game::player->y, mouse_tile->tile_x, mouse_tile->tile_y);
        if (Ui::is_targetting && mouse_tile->is_occupied())
        {
            if (distance <= spell_range)
            {
                if (Game::player->attrs->mana->current_val > mana_cost)
                {
                    // mouse_tile->occupant->combat->TakeDamage(Game::player->combat, spell_damage);
                    spell->spell_effect->ApplyAllEffects(mouse_tile->occupant);
                    if (mouse_tile->occupant != NULL) // assuming NULL if they died
                    {
                        if (spell->spell_effect->duration > 0)
                        {
                            TimedEffect* timed_effect = new TimedEffect;
                            timed_effect->effect = spell->spell_effect;
                            timed_effect->turn_applied = Game::turn_count;
                            mouse_tile->occupant->timed_spell_effects->push_back(timed_effect);
                        };
                    };
                    Game::player->attrs->mana->current_val -= mana_cost;
                    new Message(Ui::msg_handler_main, NOTYPE_MSG, "BAM casted a spell at the range of %i", distance, ".");
                    return true;
                }
                else 
                {
                    new Message(Ui::msg_handler_main, NOTYPE_MSG, "No mana for this cast!");
                };
            }
            else
            {
                new Message(Ui::msg_handler_main, NOTYPE_MSG, "Out of range. Max is %i, you're at %i.", spell_range, distance);
            };
        }
        else
        {
            new Message(Ui::msg_handler_main, NOTYPE_MSG, "Pick an actual target how about.");
        };
        //start targetting mode

    };
    return false;
};


std::map<int, generic_menu_active_t>  Input::build_spec_genmenactivemap()
{

    std::map<int, generic_menu_active_t> spec_genmenactivemap; //Keypad, punctuation
    // 
    spec_genmenactivemap[TCODK_ESCAPE] = generic_menu_active_t::EscapeGenericMenu;
    return spec_genmenactivemap;
}; //Keypad, punctuation

std::map<char, generic_menu_active_t> Input::build_char_genmenactivemap()
{

    std::map<char, generic_menu_active_t> char_genmenactivemap; //regular letters
    return char_genmenactivemap;
}; //regular letters

generic_menu_active_t Input::generic_menu_active_pressed(TCOD_key_t key)
{

    return Input::find_key(key, Input::char_genmenactivemap,
            Input::spec_genmenactivemap, generic_menu_active_t::NO_MATCHING_GENERIC_MENU_ACTIVE);
};

bool Input::process_generic_menu_keys(TCOD_key_t request)
{
    generic_menu_active_t action = Input::generic_menu_active_pressed(request);

    if( action == generic_menu_active_t::EscapeGenericMenu )
    {
        Ui::generic_active = false;
        Ui::chosen_generic = false;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Escape back to regular inventory mode.");
        return true;
    }

    return false;
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
        Item* item = ((Item*)Ui::chosen_generic);
        Ui::chosen_generic = NULL;
        Ui::generic_active = false;

        Game::player->inventory->drop_item(item);
        return true;
    }

    else if( action == inventory_items_active_t::UseItem )
    {
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Using item.");
        ((Item*)Ui::chosen_generic)->use(Game::player);
        return true;
    }

    else if( action == inventory_items_active_t::EquipItem )
    {
        ((Item*)Ui::chosen_generic)->equip(Game::player);
        Game::player->equipment->equip_item(((Item*)Ui::chosen_generic));
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Equipping item.");
        return true;
    }

    else if( action == inventory_items_active_t::UnequipItem )
    {
        ((Item*)Ui::chosen_generic)->unequip(Game::player);
        Game::player->equipment->unequip_item(((Item*)Ui::chosen_generic));
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
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "EXAMINE CLASS.");
        std::cout << "EXAMINE CLASS." << std::endl;
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

    else if( action == classes_active_t::EquipClass )
    {
        delete Game::player->actor_class;
        Game::player->actor_class = (IClass*)Ui::chosen_generic;
        Game::player->actor_class->master = Game::player;
        std::cout << "Class changed to " << ((IClass*)Ui::chosen_generic)->name << std::endl;
        // Ui::chosen_spell->equip(Game::player);
        // Game::player->equipment->equip_spell(Ui::chosen_item);
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Equipping spell.");
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
        std::cout << "EXAMINE SPELL." << std::endl;
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
        Ui::toggle_targetting();
        Ui::targetted_tile = Game::player->my_tile;
        Game::current_state = GameStates::GameplayState;
        std::cout << ((Spell*)Ui::chosen_generic)->name << std::endl;
        // Game::current_screen = Screens::Game
        //new Message(Ui::msg_handler_main, NOTYPE_MSG, "Using spell.");
        //Ui::chosen_spell->use(Game::player);
        //return true;
    }

    else if( action == spells_active_t::EquipSpell )
    {
        // Ui::chosen_spell->equip(Game::player);
        // Game::player->equipment->equip_spell(Ui::chosen_item);
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Equipping spell.");
        // return true;
    }

    else if( action == spells_active_t::UnequipSpell )
    {
        // Ui::chosen_spell->unequip(Game::player);
        // Game::player->equipment->unequip_spell(Ui::chosen_item);
        // new Message(Ui::msg_handler_main, NOTYPE_MSG, "Unequipping spell.");
        // return true;
    }

    else if( action == spells_active_t::EscapeMenuSpell )
    {
        Ui::generic_active = false;
        Ui::chosen_generic = false;
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
    std::cout << "move_target" << std::endl;
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
    Map *world = Game::world;
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
        std::cout << "WAITING" << std::endl;
        // player->is_moving_left = true;
        // if(Game::current_map->attackMovePlayer(player, -1, 0) )
        // { 
        return true;
        // }
    };

    if (has_moved) return true;


    //if the player has moved or attacked this update, increment the turn
    //counter
    if ((orig_plr_pos_x != Game::player->x || orig_plr_pos_y != Game::player->y || Game::player->has_attacked))
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

    if (request.lbutton_pressed)
    {
        std::cout << "mouse lclicked" << std::endl;
    }

    return 0;
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
        auto map = Game::build_world();
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
                if (incr_turn)
                {
                    //stop the targetting so that user has to retry
                    Ui::is_targetting = false;
                    Ui::chosen_generic = NULL;
                    Ui::generic_active = false;
                }
            }

            else if (is_key_basic_command(request))
            {
                incr_turn = Input::process_basic_keys(request);
            }

            else if(request.vk == TCODK_ESCAPE && request.pressed == 1)
            {
                std::cout << "Goodbye now" << std::endl;
                exit(1);
            }
            else
            {
                std::cout << std::endl << "command not found: " << char_to_str(request.c) << std::endl;
                std::cout << "nswe or numpad to move, i to open inventory, ESC to quit, o to open doors" << std::endl;
                std::cout << "c to open character sheet, m and k to cast spells, ? for help, > to go down" << std::endl;
            }

            break;

        case GameStates::MenuState:
            if (Game::current_screen == Screens::SpellSelectScreen)
            {
                std::vector<Spell*>* spells = Game::player->spells;
                Input::select_generic(request, spells, is_key_spell_command, Input::process_spells_keys);

            }
            else if (Game::current_screen == Screens::ClassSelectScreen)
            {
                Ui::is_targetting = false;
                std::vector<IClass*>* classes = Actor::actor_class_choices;
                Input::select_generic(request, classes, is_key_class_command, Input::process_classes_keys);
            }

            else if (Game::current_screen == Screens::InventoryScreen)
            {
                Ui::is_targetting = false;
                std::vector<Item*>* items = Game::player->inventory->items;
                Input::select_generic(request, items, is_key_inventory_command, Input::process_inventory_keys);
            }
            else 
            {
                Ui::is_targetting = false;
                printf("defaulting to generic screen\n");
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
void Input::select_generic(TCOD_key_t request, std::vector<T*>* generic_vector, bool (*active_func)(TCOD_key_t), bool (*process_func)(TCOD_key_t))
{
    int size = generic_vector->size();
    // int per_page = 10;
    // int page_num = 1;
    // int offset = Ui::per_page*Ui::page_num;
    generic_keypair_t class_map = Input::build_keypairs(size, Ui::offset);
    generic_keypair_t::iterator it = class_map.find(request.c);
    bool successful_action = true;
    if (Ui::generic_active == false)
    {
        if (request.c == 'q' && request.pressed == 1 && Ui::generic_active == false && Ui::generic_active == false && Ui::generic_active == false)
        {
            std::cout << "Back to the game." << std::endl;
            Ui::chosen_generic = NULL;
            Ui::generic_active = false;
            Game::current_state = GameStates::GameplayState;
        }
        else if ( request.c == '+'&& request.pressed == 1) 
        {
            Ui::page_num++;
            Ui::offset = Ui::per_page*Ui::page_num;
        }
        else if ( request.c == '-'&& request.pressed == 1) 
        {
            if (Ui::offset > 0)
            {
                Ui::page_num--;
                Ui::offset = Ui::per_page*Ui::page_num;
            };
        };
        //choose class
        generic_keypair_t::iterator it = class_map.find(request.c);
        if (it != class_map.end())
        {
            if ((T*)Ui::chosen_generic == generic_vector->at(it->second))
            {
                Ui::generic_active = true;
            }
            else
            {
                Ui::generic_active = false;
            };
            Ui::chosen_generic = generic_vector->at(it->second);
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
            std::cout << std::endl << "command not found: " << char_to_str(request.c) << std::endl;
            std::cout << "q to return to gameplay, a b c to choose the first, second, third class etc." << std::endl;
            std::cout << "press again to select. once it's activated, press u to use" << std::endl;
            std::cout << "e to equip, y to unequip, d to drop" << std::endl;
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
