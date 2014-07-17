#include "stdafx.h"
#include "ui.h"
//#include <cmath>

#include "game.h"
#include "input.h"
#include <actors\Person.h>
#include "utils.h"
#include "inventory.h"
#include "equipment.h"
#include "combat.h"
#include "attribute_container.h"
#include "attribute.h"
#include "item.h"
#include "attr_effect.h"
#include "messages.h"
#include "map.h"
#include "tile.h"
#include "draw_utils.h"
#include "Representation.h"
#include "spells.h"
#include "class.h"
#include "enums\hunger_threshold.h"
#include <attribute_container.h>
#include <attribute.h>
#include "screen.h"
#include "helpbox.h"
#include <color_utils.h>
#include "thinker.h"

// MessageHandler* Ui::msg_handler_main = new MessageHandler;
// Item* Ui::chosen_item = NULL;
Game* Ui::game = game;

void* Ui::chosen_generic = NULL;

bool Ui::generic_active = false;

unsigned long long int Ui::turn_checking_against = 1;
unsigned long long int Ui::last_turn_noted = 1;
unsigned long long int Ui::tick_turn_changed = 0;
unsigned long long int Ui::tick_threshold = 50;
unsigned long long int Ui::tick_checking_against = Ui::tick_threshold;

unsigned long long int Ui::tick_mouse_moved = 0;
int Ui::mouse_move_threshold = 10;
unsigned long long int Ui::tick_key_pressed = 0;
unsigned long long int Ui::tick_key_released = 0;

bool Ui::is_targetting = false;
Tile* Ui::targetted_tile = NULL;


int Ui::per_page = 10;
int Ui::page_num = 0;
int Ui::offset = Ui::per_page * Ui::page_num;

int Ui::ui_inv_w = 0;
int Ui::ui_inv_h = 0;
int Ui::ui_inv_msg_w = 0;
int Ui::ui_inv_msg_h = 0;

int Ui::ui_msg_w = 0;
int Ui::ui_msg_h = 0;
int Ui::ui_sidebar_w = 0;
int Ui::ui_sidebar_h = 0;

MessageHandler* Ui::msg_handler_main = new MessageHandler;
MessageHandler* Ui::msg_handler_inv = new MessageHandler;
MessageHandler* Ui::msg_handler_spell_select = new MessageHandler;

void Ui::update_inventory_ui()
{

};

void Ui::update_ui()
{


    // all of this is to update the turn counter
    TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);

    if (Ui::turn_checking_against < Ui::game->turn_count)
    {
        //reset the future time
        Ui::tick_checking_against = Ui::game->tick_count+Ui::tick_threshold;
        Ui::turn_checking_against = Ui::game->turn_count;

    };
    if (Ui::last_turn_noted != Ui::turn_checking_against)
    {
        //if turn was changed less than 300 ticks ago, make the turn count red
        if (Ui::game->tick_count < Ui::tick_checking_against)
        {
            unsigned long long int difference = Ui::tick_checking_against - Ui::game->tick_count;
            if (difference >= 0)
            {
                if (difference > Ui::tick_threshold)
                {
                    Ui::last_turn_noted = Ui::game->turn_count;
                    Ui::tick_turn_changed = Ui::game->tick_count;
                };
                float coef = (float)difference / (float)(Ui::tick_threshold);
                TCODColor myColor = TCODColor::lerp ( TCODColor::white, TCODColor(93,93,93), coef );
                TCODConsole::setColorControl(TCOD_COLCTRL_1, myColor, TCODColor::black);
            }
            else
            {
                TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
            };
        }
        //otherwise make it yellow because enough time has passed and reset the
        //the counters to update that we're finally in the new time
        else
        {
            TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
        };
    }
}

void Ui::draw_ui()
{
    draw_ui_sidebar();
    draw_ui_msg();
    draw_misc();
};

bool Ui::should_draw_mouse_helpbox()
{
    return Ui::tick_mouse_moved > Ui::tick_key_pressed;
};

bool Ui::should_draw_attacker_helpbox()
{
    return Game::player->combat->last_victim != NULL && Game::player->combat->last_victim->is_active;
};

void Ui::draw_status_helpbox(TCODConsole* ui_sidebar_con, Tile* target_tile, int& y)
{
    //get help text
    std::string help_text = "";
    std::string health_text = "";
    std::vector<std::string> messages;
    if (! target_tile->is_known())
    {
        help_text = "Unknown tile";
    }
    else if (target_tile->is_occupied() && Game::current_map->l_map->isInFov(target_tile->tile_x, target_tile->tile_y))
    {
        // help_text_height = 5;
        Actor* occupant = target_tile->occupant;
        help_text = occupant->name;
        float health_percent = occupant->attrs->health->GetValPercentage();
        if (health_percent > 75.0f) { health_text = "It's healthy."; }
        else if  (health_percent > 50.0f ) { health_text = "It's hurt."; }
        else if  (health_percent > 25.0f ) { health_text = "It's very hurt."; }
        else if  (health_percent > 10.0f ) { health_text = "It's in critical condition."; }
        else { health_text = "It's near death."; }

        if (occupant->thinker != NULL)
        {
            bool is_aware = occupant->thinker->get_is_aware();
            if (is_aware)
            {
                messages.push_back("Is aware of you");
            }
            else
            {
                messages.push_back("Isn't aware of you");
            };

            bool is_ally = occupant->thinker->is_ally;
            if (is_ally)
            {
                messages.push_back("Is your ally");
            }

        };

    }
    else if (target_tile->inventory->get_count() > 0)
    {
        help_text = target_tile->inventory->items->back()->name;
    }
    else
    {
        help_text = target_tile->get_description();
    };

    messages.push_back(help_text);
    if (health_text.size() != 0)
    {
        messages.push_back(health_text);
    };

    TargetHelpBox target_help_box(messages, ui_sidebar_con, target_tile);
	int x = 0;
    target_help_box.draw(x, y);

    std::vector<std::string> temp_msgs;
    temp_msgs.push_back("ASDADS");
    TargetHelpBox temp(temp_msgs, TCODConsole::root, NULL);
    temp.draw(x, y);
}


void Ui::draw_facing_angle(TCODConsole* ui_sidebar_con, int& y)
{
    //is moving in a direction
    bool ml = Game::player->is_moving_left;
    bool mr = Game::player->is_moving_right;
    bool mu = Game::player->is_moving_up;
    bool md = Game::player->is_moving_down;
    std::string sl = " ", sr = " ", st = " ", sb = " ", stl = " ", str = " ", sbl = " ", sbr = " ";

    //draw the line the right way
    if (ml && mu) { stl= "\\"; }
    else if (mr && mu) { str= "/"; }
    else if (mu) { st= "|"; }
    else if (mr && md) { sbr= "\\"; }
    else if (ml && md) { sbl= "/"; }
    else if (md) { sb= "|"; }
    else if (ml) { sl= "-"; }
    else if (mr) { sr= "-"; };

    int x_offset = 7;
    ui_sidebar_con->print(x_offset,  y++, "%s%s%s", stl.c_str(), st.c_str(), str.c_str());
    ui_sidebar_con->print(x_offset,  y++, "%so%s", sl.c_str(), sr.c_str());
    ui_sidebar_con->print(x_offset, y++, "%s%s%s", sbl.c_str(), sb.c_str(), sbr.c_str());

};

void Ui::draw_ui_sidebar()
{
    ui_sidebar_w = 20;
    ui_sidebar_h = Ui::game->screen_h-ui_msg_h;
    TCODConsole *ui_sidebar_con = new TCODConsole(ui_sidebar_w, ui_sidebar_h);

    //reset ui console to default
    TCODColor ui_sidebar_color(10, 5, 5);
    TCODColor ui_sidebar_fore = ui_sidebar_con->getDefaultForeground();
    ui_sidebar_con->setDefaultBackground(ui_sidebar_color);
    // ui_sidebar_con->clear();

    int first_y = 0;

    //draw floor
    ui_sidebar_con->print(0, first_y, "Floor %d", Game::current_map->depth);

    //draw the turn
    ui_sidebar_con->print(9, first_y, "TURN %c%d%c", TCOD_COLCTRL_1, Ui::game->turn_count, TCOD_COLCTRL_STOP);
    first_y++;

    std::string class_msg = Game::player->actor_class->name.c_str();
    if (Game::player->is_sneaking)
    { 
        class_msg.append(colfg(TCODColor::lightGrey, " Sneaking"));
    };
    ui_sidebar_con->print(0, first_y, class_msg.c_str());
    first_y++;

    //draw attributes
    Ui::draw_attrs(first_y, ui_sidebar_con);
    first_y++;
    first_y++;

    //mouse stats
    ui_sidebar_con->print(0, first_y, "MOUSE" );
    ui_sidebar_con->print(6, first_y, "%d %d", Ui::game->mouse_evt.cx+Game::camera_x, Ui::game->mouse_evt.cy+Game::camera_y);
    first_y++;
    first_y++;


    //draw player inventory info
    ui_sidebar_con->print(0, first_y, "INV:");
    ui_sidebar_con->print(5, first_y, "%d", Ui::game->player->inventory->get_count());

    //draw player burden
    ui_sidebar_con->print(10, first_y, "BUR:");
    ui_sidebar_con->print(15, first_y, "%d", Ui::game->player->inventory->get_total_weight());
    first_y++;

    Ui::draw_hunger(first_y, ui_sidebar_con);

    int initial_y = first_y++;
    Ui::draw_xp(++first_y, ui_sidebar_con, ui_sidebar_fore);
    first_y++;
    first_y++;

    if (Ui::is_targetting)
    {
        ui_sidebar_con->print(0, first_y, "SPL:");
        Spell* spell = static_cast<Spell*>(Ui::chosen_generic);
        ui_sidebar_con->print(5, first_y++, "%s", colfg(spell->get_spell_color(), spell->name).c_str());
    };

    //facing direction
    // Ui::draw_facing_angle(ui_sidebar_con, first_y);
    first_y++;

    if (Ui::should_draw_mouse_helpbox())
    {
        TCODMouse::showCursor(true);
        Ui::draw_status_helpbox(ui_sidebar_con, Game::get_mouse_tile(), first_y);
    }
    else if (Ui::should_draw_attacker_helpbox())
    {
        Ui::draw_status_helpbox(ui_sidebar_con, Game::player->combat->last_victim->my_tile, first_y);
    }
    else 
    {
        TCODMouse::showCursor(false);
    };

    //draw ui console to root
    TCODConsole::blit(ui_sidebar_con, 0, 0, ui_sidebar_w, ui_sidebar_h, TCODConsole::root, Ui::game->screen_w-ui_sidebar_w, 0 );
    delete ui_sidebar_con;
};

void Ui::draw_attrs(int& y, TCODConsole* con)
{

    AttributeContainer* attrs  = Game::player->attrs;
    TCODColor def = con->getDefaultForeground();

    //  player stats
    //generate a color for the percent of players' cur hp to max hp between red and green
    double hcv = attrs->health->current_val, hmv = attrs->health->max_val;
    float player_hp_percent = (float)hcv / (float)hmv;
    TCODColor player_hp_color = TCODColor::lerp(TCODColor::red, TCODColor::green, player_hp_percent);
    if (player_hp_percent >= 1.0) { con->setDefaultForeground(TCODColor::green); }
    else if (player_hp_percent <= 0) { con->setDefaultForeground(TCODColor::red); }
    else { con->setDefaultForeground(player_hp_color); };
    con->print(0, ++y, "HP  %d/%d", (int)hcv, (int)hmv);

    con->setDefaultForeground(TCODColor::lightBlue);
    con->print(10, y, "MAN %d/%d", (int)attrs->mana->current_val, (int)attrs->mana->max_val);

    con->setDefaultForeground(TCODColor::lightRed);
    con->print(0, ++y, "DMG %d", (int)attrs->damage->current_val);

    con->setDefaultForeground(TCODColor::darkerGrey);
    con->print(10, y, "ARM %d", (int)attrs->armor->current_val);

    con->setDefaultForeground(def);

};

void Ui::draw_hunger(int first_y, TCODConsole* console)
{
    float hunger_percentage = (float)(Game::player->attrs->hunger->current_val)/ ((float)Game::player->attrs->hunger->max_val) * 100;
    std::string hunger_message;
    if (hunger_percentage > WastingAwayHunger)
        hunger_message = "You are wasting away!";
    else if (hunger_percentage > NearDeathHunger)
        hunger_message = "You are near death!";
    else if (hunger_percentage > FamishedHunger)
        hunger_message = "You are famished!";
    else if (hunger_percentage > StarvingHunger)
        hunger_message = "You are starving!";
    else if (hunger_percentage > VeryHunger)
        hunger_message = "You are very hungry!";
    else if (hunger_percentage > HungryHunger)
        hunger_message = "You feel hungry.";
    else if (hunger_percentage > DefaultHunger)
        hunger_message = " ";
    else if (hunger_percentage > SatisfiedHunger)
        hunger_message = "You feel satiated.";
    else if (hunger_percentage > BloatedHunger)
        hunger_message = "You feel bloated.";

    console->print(0, first_y++, hunger_message.c_str());
};
bool Ui::toggle_targetting()
{
    Ui::is_targetting = !Ui::is_targetting;
    return Ui::is_targetting;
};

void Ui::draw_targetting(Tile* target_tile, int sx, int sy, int dx, int dy)
{ 
    TCODColor line_color;
    line_color = TCODColor::grey;
    if (target_tile->is_occupied() && target_tile->is_known())
    {
        Game::game_console->setChar(dx, dy, 'x');
        // TCODConsole::root->setChar(dx, dy, '&');
        // Game::game_console->setChar(target_tile->tile_x+Game::camera_x, target_tile->tile_y+Game::camera_y, 'X');
    }
    else
    {
        // line_color = TCODColor::grey;
    }

    Spell* spell = (Spell*)Ui::chosen_generic;
    int distance = get_euclidean_distance(sx, sy, dx, dy);
    actor_vec_t targets = spell->targets_around_tile(targetted_tile);
    for (actor_vec_t::iterator it = targets.begin(); it != targets.end(); it++)
    {
        Actor* actor = *it;
        if (distance < spell->max_range && Game::current_map->l_map->isInFov(actor->x, actor->y))
        {
            Game::game_console->setChar(actor->x, actor->y, 'X');
            line_color = TCODColor::darkGreen;
        };
    };

    // draw line from player to mouse
    int count = 0;
    TCODLine::init(sx, sy, dx, dy);
    do {
        count++;
        if (count > spell->max_range)
        {
            line_color = TCODColor::darkRed;
        }
        TCODColor to_draw = line_color;
        if (distance > spell->max_range)
        {
            to_draw = to_draw * TCODColor::darkestGrey;
        }
        Game::game_console->setCharBackground(sx, sy, to_draw);
    } while (!TCODLine::step(&sx,&sy));


};

void Ui::draw_misc()
{
    //spell casting
    if (Ui::is_targetting)
    {
        //bool use_mouse = true;
        bool use_mouse = Ui::should_draw_mouse_helpbox();
        if (use_mouse)
        {
            Tile* mouse_tile = Game::get_mouse_tile();
            Ui::targetted_tile = mouse_tile;
            int x = Game::player->x;
            int y = Game::player->y;
            Ui::draw_targetting(mouse_tile, x, y, mouse_tile->tile_x, mouse_tile->tile_y);
        }
        else if (!use_mouse)
        {
            Tile* mouse_tile = Ui::targetted_tile;
            int x = Game::player->x, y = Game::player->y;
            Ui::draw_targetting(mouse_tile, x, y, mouse_tile->tile_x, mouse_tile->tile_y);
        }


    }
}

void Ui::draw_xp(int& y, TCODConsole* ui_sidebar_con, TCODColor ui_sidebar_fore)
{
    // experience bar
    ui_sidebar_con->setDefaultForeground(ui_sidebar_fore);

    ui_sidebar_con->print(0, y, "LVL %d", Ui::game->player->level);

    ui_sidebar_con->print(7, y, "XP %d", Ui::game->player->xp);
    y++;
    y++;

    std::string left_exp = ">";
    std::string right_exp = "";
    float left_percent = ((float)(Ui::game->player->xp_this_level)) / Ui::game->player->xp_required_to_lvlup ;
    if (left_percent < 0) left_percent = 0.0f;
    if (left_percent > 1) left_percent = 1.0f;

    float right_raw = ((float)Ui::game->player->xp_required_to_lvlup - (float)Ui::game->player->xp_this_level);
    float right_percent = right_raw / (float)Ui::game->player->xp_required_to_lvlup;
    if (right_percent < 0) right_percent = 0.0f;
    if (right_percent > 1) right_percent = 1.0f;

    int size = 12;
    int left_count = (int)floor(left_percent * (float)size);
    int right_count = (int)floor(right_percent * (float)size);
    if (left_count + right_count < size) right_count++;

    left_exp.append(left_count, '-');
    right_exp.append(right_count, '-');
    right_exp.append(1, '>');

    int padding = 2;
    ui_sidebar_con->print(padding, y, std::string(colfg(TCODColor::lighterBlue, "%s")+"%s").c_str(),  left_exp.c_str(),  right_exp.c_str());
    y++;

};

void Ui::draw_ui_msg()
{
    ui_msg_w = Ui::game->screen_w;
    ui_msg_h = 10;
    TCODConsole *ui_msg_con = new TCODConsole(ui_msg_w, ui_msg_h);

    //reset ui console to default
    TCODColor ui_msg_color(12,12,12);
    ui_msg_con->setDefaultBackground(ui_msg_color);
    ui_msg_con->clear();

    //draw the message text
    Ui::msg_handler_main->draw(ui_msg_con);

    //draw ui console to root
    TCODConsole::blit(ui_msg_con, 0, 0, ui_msg_w, ui_msg_h, TCODConsole::root, 0, Ui::game->screen_h-ui_msg_h);
    delete ui_msg_con;
};

void Ui::draw_inventory_main()
{
    Ui::draw_inventory_ui();
    Ui::draw_inventory_msg();
};

void Ui::draw_class_select_main()
{
    Ui::draw_class_select_ui();
    Ui::draw_class_select_msg();
};

void Ui::draw_spell_select_main()
{
    Ui::draw_spell_select_ui();
    Ui::draw_spell_select_msg();
};

void Ui::draw_screen_title(std::string title, TCODConsole* con)
{
    int inv_title_x = Ui::game->screen_w/2;
    TCOD_bkgnd_flag_t bkgnd_flag = con->getBackgroundFlag();
    con->printEx(inv_title_x, 2, bkgnd_flag, TCOD_alignment_t::TCOD_CENTER, title.c_str());

};

TCODConsole* Ui::create_screen()
{
    int con_w = Ui::game->screen_w;
    int con_h = Ui::game->screen_h - 10;
    TCODConsole *con = new TCODConsole(con_w, con_h);
    return con;
};

void Ui::format_attribute(Attribute* attr, char buffer[])
{
    std::string msg_template = "%d out of %d, regenerating %d every %d turns";
    sprintf(buffer, msg_template.c_str(), (int)attr->current_val, (int)attr->max_val, (int)attr->regen_rate, (int)attr->regen_interval);
};

void Ui::print_attribute(TCODConsole* con, Attribute* attr, char buffer[], int& offset, int& i, std::string name)
{
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, name.c_str());
    i++;
    Ui::format_attribute(attr, buffer);
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, buffer);
    i++;
    i++;
};

void Ui::print_experience(TCODConsole* con, int& offset, int& i)
{
    char buffer[100];
    Person* player = Game::player;
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, "EXPERIENCE");
    i++;
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, "Level %i", player->level);
    i++;

    std::string msg_template = "%i out of %i XP to next level, with %i total.";
    sprintf(buffer, msg_template.c_str(), player->xp_this_level, player->xp_required_to_lvlup, player->xp);
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, buffer);
    i++;
    i++;
};

void Ui::print_class(TCODConsole* con, int& offset, int& i)
{
    // char buffer[100];
    Person* player = Game::player;
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, "CLASS");
    i++;
    con->printEx(offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, "%s", player->actor_class->name.c_str());
    i++;
    i++;

    //std::string msg_template = "%i out of %i XP to next level, with %i total.";
    //sprintf(buffer, msg_template.c_str(), player->xp_this_level, player->xp_required_to_lvlup, player->xp);
    //con->printEx(3, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET, TCOD_alignment_t::TCOD_LEFT, buffer);
};

    template<typename T1, typename T2>
void print_keys_for(TCODConsole* con, int offset, int& i, std::map<char, T1> char_map,std::map<int, T2> spec_map, std::vector<std::string> string_map, std::string title)
{
    con->print(offset, i++, "__ %s", title.c_str());
    i++;

    std::map<char, T1> char_commands = char_map;
    for (std::map<char, T1>::iterator it=  char_commands.begin(); it != char_commands.end(); ++it)
    {
        char key = it->first;
        T1 val = it->second;
        std::string text = string_map.at((int)val);
        con->print(offset, i++, "%c -> %s", key, text.c_str());

    }

    std::map<int, T2> spec_commands = spec_map;
    for (std::map<int, T2>::iterator it=  spec_commands.begin(); it != spec_commands.end(); ++it)
    {
        int ikey = it->first;
        auto keymap = Input::get_tcodkey_to_string_map();
        std::string key = keymap[ikey];
        T2 val = it->second;
        std::string text = string_map.at((int)val);
        con->print(offset, i++, "%s -> %s", key.c_str(), text.c_str());
    }


};

void Ui::main_menu_screen_ui_loop(TCODConsole* con, int offset, int i, char key)
{
    // Ui::class_ui_loop(con, offset, i, key);
    // print_keys_for(con, offset, i, Input::char_active_map, Input::spec_active_map, Input::basic_cmds_char, "Gameplay Keys");
    // i++;
    // print_keys_for(con, offset, i, Input::char_invitemactivemap, Input::spec_invitemactivemap, Input::inventory_items_active_char, "Inventory Keys");
    // i++;
    // print_keys_for(con, offset, i, Input::char_spellactivemap, Input::spec_spellactivemap, Input::spells_active_char, "Spell Selection Keys");
};

void Ui::help_screen_ui_loop(TCODConsole* con, int offset, int i, char key)
{
    print_keys_for(con, offset, i, Input::char_main_keymap, Input::spec_main_keymap, Input::basic_cmds_char, "Gameplay Keys");
    i++;
    print_keys_for(con, offset, i, Input::char_inventory_keymap, Input::spec_inventory_keymap, Input::inventory_items_active_char, "Inventory Keys");
    i++;
    print_keys_for(con, offset, i, Input::char_spell_select_keymap, Input::spec_spell_select_keymap, Input::spells_active_char, "Spell Selection Keys");
};

void Ui::character_sheet_ui_loop(TCODConsole* con, int offset, int i, char key)
{
    AttributeContainer* player_attrs = Game::player->attrs;

    HealthAttribute* health = player_attrs->health;
    ManaAttribute* mana = player_attrs->mana;
    DamageAttribute* damage = player_attrs->damage;
    ArmorAttribute* armor = player_attrs->armor;
    HungerAttribute* hunger = player_attrs->hunger;

    char buffer [100];
    Ui::print_attribute(con, health, buffer, offset, i, "HEALTH");
    Ui::print_attribute(con, mana, buffer, offset, i, "MANA");
    Ui::print_attribute(con, damage, buffer, offset, i, "DAMAGE");
    Ui::print_attribute(con, armor, buffer, offset, i, "ARMOR");
    // Ui::print_attribute(con, hunger, buffer, offset, i, "HUNGER");

    print_experience(con, offset,  i);
    print_class(con, offset,  i);

};




void Ui::draw_screen(std::string title, void (*loop_through_lines)(TCODConsole*, int, int, char))
{
    // clear the screen
    TCODConsole::root->clear();

    TCODConsole *con = Ui::create_screen();
    Ui::draw_screen_title(title, con);

    // draw the list of items on the player
    int offset = 5;
    int i = offset;
    char key = 'a';

    //draw mouse line
    int hline = 2;
    con->hline(0, Game::mouse_evt.cy, hline);
    con->putChar(hline, Game::mouse_evt.cy, '>');


    if (loop_through_lines != NULL)
    {
        loop_through_lines(con, offset, i, key);
    };

    TCODConsole::blit(con, 0, 0, ui_inv_w, ui_inv_h, TCODConsole::root, 0, 0);
    delete con;

};

void Ui::draw_inventory_ui()
{
    InventoryScreen<Item> inv_screen;
    inv_screen.elements = Game::player->inventory->items;
    // inv_screen->msg_hdlr = Ui::msg_handler_inv;
    inv_screen.draw();
};

void Ui::draw_spell_select_ui()
{
    SpellScreen<Spell> spell_screen;
    spell_screen.elements = Game::player->spells;
    // spell_screen->msg_hdlr = Ui::msg_handler_spell_select;
    spell_screen.draw();
};

void Ui::draw_class_select_ui()
{
    ClassScreen<IClass> class_screen;
    class_screen.elements = Game::player->actor_class_choices;
    class_screen.draw();
};

void Ui::draw_main_menu_ui()
{
    Ui::draw_screen("Main menu", &Ui::main_menu_screen_ui_loop);
};

void Ui::draw_char_sheet_ui()
{
    Ui::draw_screen("Character Sheet", &Ui::character_sheet_ui_loop);
};

void Ui::draw_help_ui()
{
    Ui::draw_screen("Help Information", &Ui::help_screen_ui_loop);
};

void one_line_helper(TCODConsole* con,int& offset, int& i, std::string msg_str, std::vector<TCOD_colctrl_t> color_vector)
{
    //add a col stop to end
    if (color_vector.size() != 0)
        msg_str.append("%c");
    auto msg = msg_str.c_str();
    // char to_print[500];
    // std::strcpy(to_print, msg_str.c_str());

    // std::ostringstream out;
    // out << msg_str;


    // auto it = color_vector.begin();
    // for (it; it != color_vector.end(); it++)
    // {
    //const char* const_char_ptr = &to_print[0];
    //sprintf(to_print, const_char_ptr, (*it));
    // out << (*it);

    // printf(std::string(std::string(out.str())+"\n").c_str());
    // };
    // out << TCOD_COLCTRL_STOP;
    //sprintf(to_print, &to_print[0], TCOD_COLCTRL_STOP);
    int x = offset;
    // con->print(x, i, out.str().c_str());

    if (color_vector.size() == 0)
        con->print(x, i, msg);
    else if (color_vector.size() == 1)
        con->print(x, i, msg, color_vector.at(0), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 2)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 3)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 4)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 5)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 6)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 7)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 8)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 9)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 10)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 11)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 12)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), color_vector.at(11), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 13)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), color_vector.at(11), color_vector.at(12), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 14)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), color_vector.at(11), color_vector.at(12), color_vector.at(13), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 15)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), color_vector.at(11), color_vector.at(12), color_vector.at(13), color_vector.at(14), TCOD_COLCTRL_STOP);
    else if (color_vector.size() == 16)
        con->print(x, i, msg, color_vector.at(0), color_vector.at(1), color_vector.at(2), color_vector.at(3), color_vector.at(4), color_vector.at(5), color_vector.at(6), color_vector.at(7), color_vector.at(10), color_vector.at(11), color_vector.at(12), color_vector.at(11), color_vector.at(12), color_vector.at(13), color_vector.at(14), color_vector.at(15), TCOD_COLCTRL_STOP);
};

void Ui::draw_class_select_msg()
{
    ui_inv_msg_w = Ui::game->screen_w;
    ui_inv_msg_h = 10;
    TCODConsole *ui_inv_msg_con = new TCODConsole(ui_inv_msg_w, ui_inv_msg_h);

    //reset ui console to default
    TCODColor ui_inv_msg_color(12,12,12);
    ui_inv_msg_con->setDefaultBackground(ui_inv_msg_color);
    ui_inv_msg_con->clear();

    //draw the message text
    int y = 0;
    ui_inv_msg_con->setDefaultForeground(TCODColor::lightGrey+TCODColor::yellow);
    ui_inv_msg_con->print(0, y++, "Press the desired class's letter once to select it, and once more to confirm.");
    y++;
    ui_inv_msg_con->print(0, y++, "To confirm a selection, press 'a' for the first class on screen, 'b' for the");
    ui_inv_msg_con->print(0, y++, "second etc. Once you're done, press 'q' to quit the menu and play.");
    y++;

    // ui_inv_msg_con->print(0, y++, "Use corpses and potions, equip swords and helms.");
    // ui_inv_msg_con->print(0, y++, "You need a free slot to equip anything, naturally.");

    ui_inv_msg_con->setDefaultForeground(TCODColor::white);
    if (Ui::item_is_chosen())
    {
        IClass* chosen_class = (IClass*)Ui::chosen_generic;
        std::string class_name = chosen_class->name;
        ui_inv_msg_con->print(2, y++, "%s", class_name.c_str());
    }
    else
    {
        ui_inv_msg_con->print(2, y++, "No class highlighted");

    };
    // ui_inv_msg_con->print(2, y++, "Class chosen? %s", BoolToString(Ui::class_is_chosen()));
    if (!Ui::generic_active)
    {
        // IClass* chosen_class = (IClass*)Ui::chosen_generic;
        // std::string class_name = chosen_class->name;
        // ui_inv_msg_con->print(2, y++, "Press '%c' again to enter actions mode to equip %s", Input::generic_index, class_name.c_str());
        ui_inv_msg_con->print(2, y++, "Enter the class actions menu by pressing the %c key, or Enter, once more.", Input::generic_index);
    }
    else if (Ui::generic_active)
    {
        y++;

        ui_inv_msg_con->setDefaultForeground(TCODColor::lightGrey+TCODColor::yellow);
        std::string equip_msg = colfg(TCODColor::white, "e to equip");
        std::string examine_msg = colfg(TCODColor::white, "x to examine");
        std::string message(std::string("Now you can press "+equip_msg+" the class or "+examine_msg+" it."));
        ui_inv_msg_con->print(0, y++, message.c_str());
        y++;
    };

    ui_inv_msg_con->setDefaultForeground(TCODColor::white);

    //draw ui console to root
    TCODConsole::blit(ui_inv_msg_con, 0, 0, ui_inv_msg_w, ui_inv_msg_h, TCODConsole::root, 0, Ui::game->screen_h-ui_inv_msg_h);
    delete ui_inv_msg_con;

};

void Ui::draw_spell_select_msg()
{
    ui_inv_msg_w = Ui::game->screen_w;
    ui_inv_msg_h = 10;
    TCODConsole *ui_inv_msg_con = new TCODConsole(ui_inv_msg_w, ui_inv_msg_h);

    //reset ui console to default
    TCODColor ui_inv_msg_color(12,12,12);
    ui_inv_msg_con->setDefaultBackground(ui_inv_msg_color);
    ui_inv_msg_con->clear();

    //draw the message text
    int y = 0;
    ui_inv_msg_con->setDefaultForeground(TCODColor::lightGrey+TCODColor::yellow);
    ui_inv_msg_con->print(0, y++, "Press the desired spell's letter once to select it, and once more to confirm");
    std::string equip_msg = colfg(TCODColor::white, "c to cast");
    std::string examine_msg = colfg(TCODColor::white, "x to examine");
    std::string message(std::string("You can then press "+equip_msg+" it or "+examine_msg+" it."));
    ui_inv_msg_con->print(0, y++, message.c_str());
    y++;
    // ui_inv_msg_con->print(0, y++, "Use corpses and potions, equip swords and helms.");
    // ui_inv_msg_con->print(0, y++, "You need a free slot to equip anything, naturally.");

    ui_inv_msg_con->setDefaultForeground(TCODColor::white);
    y++;
    ui_inv_msg_con->print(2, y++, "Spell chosen? %s", BoolToString(Ui::spell_is_chosen()));
    ui_inv_msg_con->print(2, y++, "Spell active? %s", BoolToString(Ui::generic_active));

    //draw ui console to root
    TCODConsole::blit(ui_inv_msg_con, 0, 0, ui_inv_msg_w, ui_inv_msg_h, TCODConsole::root, 0, Ui::game->screen_h-ui_inv_msg_h);
    delete ui_inv_msg_con;
};

void Ui::draw_inventory_msg()
{
    ui_inv_msg_w = Ui::game->screen_w;
    ui_inv_msg_h = 10;
    TCODConsole *ui_inv_msg_con = new TCODConsole(ui_inv_msg_w, ui_inv_msg_h);

    //reset ui console to default
    TCODColor ui_inv_msg_color(12,12,12);
    ui_inv_msg_con->setDefaultBackground(ui_inv_msg_color);
    ui_inv_msg_con->clear();

    //draw the message text
    int y = 0;
    ui_inv_msg_con->setDefaultForeground(TCODColor::lightGrey+TCODColor::yellow);
    if (!Ui::generic_active)
    {
        TCODColor chosen_col = ui_inv_msg_con->getDefaultForeground();
        if (!Ui::item_is_chosen())
        {
            chosen_col = TCODColor::darkerYellow;
        };
        TCODColor active_col = ui_inv_msg_con->getDefaultForeground();
        if (Ui::item_is_chosen())
        {
            active_col = TCODColor::darkerYellow;
        };
        std::string choose_msg = colfg(chosen_col, "desired item's letter once to select it");
        std::string active_msg = colfg(active_col, "and again to confirm");
        ui_inv_msg_con->print(0, y++, std::string("Press the "+choose_msg+", "+active_msg+" ").c_str());
        ui_inv_msg_con->print(0, y++, "your selection. Otherwise, press q to unchoose or quit back to game.");
        y++;
        ui_inv_msg_con->print(0, y++, "IE 'a' for the first item on screen, 'b' for the second etc");
    }
    else if (Ui::generic_active)
    {
        std::string equip_msg = colfg(TCODColor::white, "e to equip");
        std::string use_msg = colfg(TCODColor::white, "u to use");
        std::string drop_msg = colfg(TCODColor::white, "d to drop");
        std::string quit_msg = colfg(TCODColor::white, "q to deselect/quit");
        std::string examine_msg = colfg(TCODColor::white, "y to examine");
        std::string zap_msg = colfg(TCODColor::white, "z to zap");
        std::string message("You can press "+equip_msg+" it or "+use_msg+" it, "+drop_msg+" it,");
        std::string message2(quit_msg+", "+examine_msg+" it, "+zap_msg+" it.");
        ui_inv_msg_con->print(0, y++, message.c_str());
        ui_inv_msg_con->print(0, y++, message2.c_str());
        y++;
        ui_inv_msg_con->print(0, y++, "Use corpses and potions, equip swords and helms.");
        ui_inv_msg_con->print(0, y++, "You need a free slot to equip anything, naturally.");
    };

    ui_inv_msg_con->setDefaultForeground(TCODColor::white);
    y++;
    // ui_inv_msg_con->print(2, y++, "Item chosen? %s", BoolToString(Ui::item_is_chosen()));
    if (Ui::item_is_chosen())
    {
        ui_inv_msg_con->print(2, y++, "%s", Ui::item_display_line().c_str());
    }
    else
    {
        ui_inv_msg_con->print(2, y++, "No item chosen");

    };
    // ui_inv_msg_con->print(2, y++, "Item active? %s", BoolToString(Ui::generic_active));

    //draw ui console to root
    TCODConsole::blit(ui_inv_msg_con, 0, 0, ui_inv_msg_w, ui_inv_msg_h, TCODConsole::root, 0, Ui::game->screen_h-ui_inv_msg_h);
    delete ui_inv_msg_con;
};

std::string Ui::item_display_line()
{
    std::stringstream ss;
    Item* item = (Item*)Ui::chosen_generic;
    ss << item->name << ".";
    if (Game::player->equipment->is_item_equipped(item))
    {
        ss << " is equipped";
    }
    else if (item->equippable)
    {
        ss << " is equippable";
    }
    else if (item->usable)
    {
        ss << " uses left: " << item->uses;
    }

    return ss.str();
};

bool Ui::item_is_chosen()
{
    return Ui::chosen_generic != NULL;
};

bool Ui::class_is_chosen()
{
    return Ui::chosen_generic != NULL;
};

bool Ui::spell_is_chosen()
{
    return Ui::chosen_generic != NULL;
};

void Ui::draw_main_menu()
{
    printf("hello!\r");
};

void Ui::reset_generic()
{
    Ui::chosen_generic = NULL;
    Ui::generic_active = false;
};
