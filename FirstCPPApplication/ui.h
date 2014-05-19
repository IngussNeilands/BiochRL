#pragma once
#pragma comment (lib, "FirstCPPApplication.lib")
#ifndef UI_H
#define UI_H

#include "libtcod_cpp_hpp\libtcod.hpp"
#include "messages.h"

class Item;
class Game;
class Tile;
class TCODColor;
class Attribute;
class TCODConsole;
class Spell;
class IClass;

void one_line_helper(TCODConsole* ui_inv_con,int& offset, int& i, std::string msg_str, std::vector<TCOD_colctrl_t> color_vector);

class Ui
{
    // private:
    public:
        static unsigned long long int tick_threshold;
        static unsigned long long int tick_turn_changed;
        static unsigned long long int last_turn_noted;
        static unsigned long long int turn_checking_against;
        static unsigned long long int tick_checking_against;

        static unsigned long long int tick_mouse_moved;
        static int mouse_move_threshold;
        static unsigned long long int tick_key_pressed;
        static unsigned long long int tick_key_released;

        static bool is_targetting;
        static Tile* targetted_tile;
        static void draw_targetting(Tile* target_tile, int sx, int sy, int dx, int dy, int lx, int ly);

        static int per_page;
        static int page_num;
        static int offset;

        static Game* game;
        static MessageHandler* msg_handler_main;
        static MessageHandler* msg_handler_inv;
        static MessageHandler* msg_handler_spell_select;

        static int ui_inv_w;
        static int ui_inv_h;
        static int ui_inv_msg_w;
        static int ui_inv_msg_h;

        static int ui_msg_w;
        static int ui_msg_h;
        static int ui_sidebar_w;
        static int ui_sidebar_h;

        static void* chosen_generic;
        static bool generic_active;

        static void update_ui();
        static void update_inventory_ui();

        static void draw_ui();
        static void draw_ui_msg();
        static void draw_ui_sidebar();
        static void draw_misc();
        static void draw_facing_angle(TCODConsole* ui_sidebar_con, int& y);
        static bool should_draw_mouse_helpbox();
        static void draw_mouse_helpbox();
        static void draw_xp();
        static void draw_xp(int& y, TCODConsole* ui_sidebar_con, TCODColor ui_sidebar_fore);
        static void draw_hunger(int y, TCODConsole* ui_sidebar_con);
        static void draw_attrs(int& y, TCODConsole* con);

        static TCODConsole* create_screen();
        static void draw_screen_title(std::string title, TCODConsole* con);
        static void draw_screen(std::string title, void (*loop_through_lines)(TCODConsole*, int, int, char));

        static void draw_main_menu_ui();
        static void main_menu_screen_ui_loop(TCODConsole* con, int offset, int i, char key);

        static void format_attribute(Attribute* attr, char buffer[]);
        static void print_attribute(TCODConsole* con, Attribute* attr, char buffer[], int& offset, int& i, std::string name);
        static void print_experience(TCODConsole* con, int& offset, int& i);
        static void print_class(TCODConsole* con, int& offset, int& i);
        static void draw_char_sheet_ui();
        static void character_sheet_ui_loop(TCODConsole* con, int offset, int i, char key);
        static void help_screen_ui_loop(TCODConsole* con, int offset, int i, char key);

        static void Ui::spell_ui_loop(TCODConsole* con, int offset, int i, char key);
        static void draw_spell_select_ui();
        static void Ui::draw_spell_select_main();
        static void Ui::draw_spell_select_msg();

        static void Ui::draw_help_ui();

        static void draw_class_select_main();
        static void draw_class_select_ui();
        static void Ui::draw_class_select_msg();
        static void class_ui_loop(TCODConsole* con, int offset, int i, char key);

        static void draw_inventory_main();
        static void draw_inventory_ui();
        static void draw_inventory_msg();
        static void inventory_ui_loop(TCODConsole* con, int offset, int i, char key);

        static bool item_is_chosen();
        static bool Ui::spell_is_chosen();
        static bool Ui::class_is_chosen();
        static bool toggle_targetting();

        static void Ui::draw_main_menu();
};

#endif
