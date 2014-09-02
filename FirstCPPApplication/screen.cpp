#include "stdafx.h"
#include "screen.h"
#include <game.h>
#include <ui.h>
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
#include "screenitem.h"
#include "custom_key.h"
#include "menu_item.h"


    template<typename T>
Screen<T>::Screen() 
{
    this->title = "Untitled Screen";
    this->empty_msg =  "There's nothing here. Try levelling up or picking up items!";

    this->offset = 5;
    this->key = 'a';
    this->hline = 2;

    // this->msg_hdlr = NULL;
    this->elements = new std::vector<T*>;
    this->screen_items = new std::vector<ScreenItem*>;
};

    template<typename T>
int Screen<T>::get_custom_key_index(T* element) 
{
    if (element->custom_key != NULL)
    {
        CustomKey* ck = element->custom_key;
        if (ck == Game::custom_key1) { return 1; }
        else if (ck == Game::custom_key2) { return 2; }
        else if (ck == Game::custom_key3) { return 3; }
        else if (ck == Game::custom_key4) { return 4; }
        else if (ck == Game::custom_key5) { return 5; }
        else { return -1; }
    };
};

    template<typename T>
bool Screen<T>::is_chosen(T* element)
{
    return element == Ui::chosen_generic;
};

    template<typename T>
bool Screen<T>::is_active(T* element)
{
    return Ui::generic_active;
};

// InventoryScreen::InventoryScreen() : Screen()
// {
// };

    template<typename T>
void Screen<T>::draw_screen_title(TCODConsole* con)
{
    int inv_title_x = Ui::game->screen_w/2;
    TCOD_bkgnd_flag_t bkgnd_flag = con->getBackgroundFlag();
    con->printEx(inv_title_x, 2, bkgnd_flag, TCOD_alignment_t::TCOD_CENTER, title.c_str());
};


    template<typename T>
void Screen<T>::draw_mouse_horiz_line(TCODConsole* con)
{
    //draw mouse line
    con->hline(0, Game::mouse_evt.cy, this->hline);
    con->putChar(this->hline, Game::mouse_evt.cy, '>');

};

    template<typename T>
TCODConsole* Screen<T>::create_screen()
{
    int con_w = Game::screen_w;
    int con_h = Game::screen_h - 10;
    TCODConsole *con = new TCODConsole(con_w, con_h);
    return con;
};

    template<typename T>
void Screen<T>::draw()
{

    TCODConsole::root->clear();

    TCODConsole *con = this->create_screen();
    this->draw_screen_title(con);

    //draw mouse line
    this->draw_mouse_horiz_line(con);

    int i = 5;
    this->loop(con, i);

    TCODConsole::blit(con, 0, 0, Ui::ui_inv_w, Ui::ui_inv_h, TCODConsole::root, 0, 0);
    delete con;

};

    template<typename T>
void Screen<T>::draw_msg()
{

    // TCODConsole::root->clear();

    // TCODConsole *con = this->create_screen();
    // this->draw_screen_title(con);

    // //draw mouse line
    // this->draw_mouse_horiz_line(con);

    // int i = 5;
    // this->loop(con, i);

    // TCODConsole::blit(con, 0, 0, Ui::ui_inv_w, Ui::ui_inv_h, TCODConsole::root, 0, 0);
    // delete con;

};

    template<typename T>
void Screen<T>::draw_scroll_arrows(TCODConsole* con, bool left_to_draw)
{
    int x = 65;
    int line_height = 15;
    int offset = 11;

    char top_char = ' ';
    if (Ui::offset != 0) { top_char = '^'; }
    con->putChar(x, 10, top_char);

    for (int i = 0; i < line_height/2; i++)
    {
        con->putChar(x, i+offset, '.');
    };

    con->print(x, offset+(line_height/2), "%i", Ui::offset/10+1);

    for (int i = 0; i < line_height/2; i++)
    {
        con->putChar(x, 1+i+offset+(line_height/2), '.');
    };

    char bot_char = ' ';
    if ( left_to_draw ) { bot_char = 'v'; }
    else  { bot_char = '-'; }
    con->putChar(x, 26, bot_char);

}

    template<typename T>
void Screen<T>::draw_empty_msg(TCODConsole* con, int i)
{
    con->print(this->offset, i, this->empty_msg.c_str());
};
    template<typename T>
void Screen<T>::build_screen_items(TCODConsole* con, int i)
{
    if (this->elements->empty())
    {
        this->draw_empty_msg(con, i);
    };

    int item_count = 0;
    if (this->elements->size() < Ui::offset) //TODO:: give each screen its own offset
    {
        Ui::offset = 0;
    }

    std::vector<T*>::iterator it = this->elements->begin() + Ui::offset;
    for (it; it != this->elements->end() && it - this->elements->begin() != (Ui::offset + Ui::per_page); ++it) 
    {
        ScreenItem si = this->build_screen_item(con, i, *it);
        this->draw_screen_item(con, i, si);
        si.handle_mouse(i);

        this->key++;
        item_count++;
        if (item_count >= Ui::per_page) { break; };
    }
    if (item_count >= Ui::per_page || Ui::offset != 0)
    {
        bool left_to_draw = it != this->elements->end();
        Screen::draw_scroll_arrows(con, left_to_draw);
    }

};


    template<typename T>
void Screen<T>::loop(TCODConsole* con, int i)
{
    TCODColor foreground, background;
    foreground = TCODColor::white;

    bool is_chosen, is_active;
    std::vector<T*>* v  = this->elements;
    int num_elems = v->size();

    if (Ui::offset >= num_elems && num_elems > 0) //if the offset is passed the end of the list
    {
        Ui::page_num = std::ceil(((float)num_elems/(float)Ui::per_page))-1; //round up and then take off a page
        Ui::offset = Ui::page_num*Ui::per_page;
    };

    this->build_screen_items(con, i);
};

    template<typename T>
std::vector<TCODColor> Screen<T>::get_colors(TCODConsole* con, T* element)
{
    TCODColor foreground, background;
    if (this->is_chosen(element))
    {
        if (this->is_active(element))
        { 
            foreground = TCODColor::red+TCODColor::yellow; 
        }
        else { foreground = TCODColor::red+TCODColor::green; }
    }
    else
    {
        foreground = TCODColor::white;
    };

    background = con->getDefaultBackground();

    std::vector<TCODColor> result;
    result.push_back(foreground);
    result.push_back(background);
    return result;


};

bool InventoryScreen::is_enabled(Item* element)
{
    return Ui::game->player->equipment->is_item_equipped(element);
};

std::string strip_tcodcolor(std::string in_string)
{
    if (in_string.c_str()[0] == TCOD_COLCTRL_FORE_RGB)
    {
        return in_string.substr(4, in_string.size()-1);
    }
    else
    {
        return in_string;
    };
}

ScreenItem InventoryScreen::build_screen_item(TCODConsole* con, int i, Item* element)
{

    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);
    bool is_enabled = this->is_enabled(element);

    ScreenItem result;
    result.key = this->key;
    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_enabled_colors(con,  element);
    foreground = colors.at(0);
    background = colors.at(1);

    std::stringstream ss;
    std::string key = char_to_str(this->key);
    std::string letter = colfg(*element->repr->fg_color, char_to_str(element->repr->repr));
    std::string name = colfg(foreground, strip_tcodcolor(element->name));
    std::string weight = colfg(TCODColor::lightGrey, std::string("weighs ")+std::to_string((long double)element->weight)+" stones");
    std::string range = "";
    if (element->range != 1)
    {
        range = colfg(TCODColor::lightGrey, ", reach of ")+colfg(TCODColor::lightGrey, element->range);
    }

    ss << key << "-" << letter << " " << name << " : " << weight << range;

    if (is_chosen)
    {
        ss << " <-";
    }
    if (element->custom_key != NULL)
    {
        // std::string index = std::to_string((long double)this->get_custom_key_index(element));
        std::string index = std::to_string((long double)element->custom_key->index);
        ss << colfg(TCODColor::green, " "+index);
    };


    if (element->spell_effect != NULL) //TODO get spell effects showing
    {
        Spell* spell = element->spell_effect;
        std::string spell_name = spell->name;
        int mana_cost = spell->mana_cost;
        int range = spell->max_range;
        ss << " @ " << spell_name << ", mana: " << mana_cost << ", rng: " << range;

    };

    std::string msg_str = ss.str();
    result.foreground = foreground;
    result.background = background;
    result.msg_str = msg_str;
    result.element = element;

    return result;
};

    template<typename T>
std::vector<TCODColor> Screen<T>::get_enabled_colors(TCODConsole* con, T* element)
{
    TCODColor foreground, background;

    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);
    bool is_enabled = this->is_enabled(element);

    if (is_enabled)
    {
        background = TCODColor::darkestRed;
        if (is_chosen)
        {
            if (is_active) 
            { 
                // foreground = TCODColor::red+TCODColor::blue; 
                foreground = TCODColor::lerp(TCODColor::red, TCODColor::blue, 0.5f);
            }
            else 
            {
                foreground = TCODColor::red+TCODColor::green; 
            }
        }
        else { foreground = TCODColor::red;
        };
    }
    else
    {
        background = con->getDefaultBackground();
        if (is_chosen)
        {
            if (is_active) 
            {
                // foreground = TCODColor::red-TCODColor::yellow; 
                foreground = TCODColor::lerp(TCODColor::red, TCODColor::yellow, 0.5f);
            }
            else 
            {
                foreground = TCODColor::red+TCODColor::green; 
            }
        }
        else
        {
            foreground = TCODColor::white;
        };
    };

    std::vector<TCODColor> result;
    result.push_back(foreground);
    result.push_back(background);
    return result;
};

void InventoryScreen::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    TCODConsole::setColorControl(TCOD_COLCTRL_1, si.foreground, si.background);
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char);

    i++;

    //print the item effects
    con->print(this->offset, i, ((Item*)si.element)->attr_effect->oneline_str().c_str());
    si.max_y = i;
    i++;
    i++;
};



/* SPELL SCREEN */
ScreenItem SpellScreen::build_screen_item(TCODConsole* con, int i, Spell* element)
{
    ScreenItem result;
    result.key = this->key;

    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);
    bool has_duration = element->attr_effect->duration != -1;

    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_colors(con, element);
    foreground = colors.at(0);
    background = colors.at(1);

    TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lightCyan, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::white, background);

    std::stringstream ss;
    std::string key = char_to_str(this->key);
    std::string name = colfg(foreground, strip_tcodcolor(element->name));
    std::string letter = colfg(element->get_spell_color(), "s");
    std::stringstream mana_cost_str;
    mana_cost_str << element->mana_cost;
    std::string mana_cost = colfg(TCODColor::lightCyan, mana_cost_str.str());
    std::string range = colfg(TCODColor::white, element->max_range);

    //key, element, name
    std::string base_msg_str = "%c-%c%c%c %c%s%c : ";
    ss << key << "-" << letter << " " << name << " : ";
    //mana, range
    ss << mana_cost << " mana, " << range << "rng";

    //duration
    if (has_duration)
    {
        ss << ", " << element->attr_effect->duration << "dur";
    };

    //aoe
    if (element->aoe > 0)
    {
        ss << ", " << element->aoe << "aoe";
    };

    if (element->custom_key != NULL)
    {
        // std::string index = std::to_string((long double)this->get_custom_key_index(element));

        std::string index = std::to_string((long double)element->custom_key->index);
        std::string msg_str = std::string(colfg(TCODColor::green, " "+index));
        ss << msg_str;
    };

    if (is_chosen)
    {
        ss << " <-";
    }

    result.foreground = foreground;
    result.background = background;
    result.msg_str = ss.str();
    result.element = element;

    return result;
};

void SpellScreen::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char, this->key );
    i++;

    //print the item effects
    con->print(this->offset, i, ((Spell*)si.element)->attr_effect->oneline_str().c_str());
    si.max_y = i;

    i++;
    i++;
};

/* SPELL SCREEN */
ScreenItem ClassScreen::build_screen_item(TCODConsole* con, int i, IClass* element)
{
    ScreenItem result;
    result.key = this->key;


    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_enabled_colors(con, element);
    foreground = colors.at(0);
    background = colors.at(1);

    TCODConsole::setColorControl(TCOD_COLCTRL_1, foreground, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::lightGrey+TCODColor::yellow, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lightCyan, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::white, background);

    //key, element, name
    std::stringstream ss;
    ss << this->key << "-" << colfg(foreground, element->name);

    if (this->is_chosen(element))
    {
        ss << " <-";
    }

    result.foreground = foreground;
    result.background = background;
    result.msg_str = ss.str();
    result.element = element;

    return result;
};

bool ClassScreen::is_enabled(IClass* element)
{
    return Ui::game->player->actor_class->type == element->type;
};


void ClassScreen::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char);
    i++;

    char buffer[512];
    std::string msg_str = "%c%s%c";
    sprintf(buffer, msg_str.c_str(), TCOD_COLCTRL_2,
            ((IClass*)si.element)->description.c_str(), TCOD_COLCTRL_STOP);
    msg_str = buffer;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_str.c_str());
    si.max_y = i;

    i++;
    i++;
};

/* MAIN MENU SCREEN */
ScreenItem MainMenuScreen::build_screen_item(TCODConsole* con, int i, std::string* element)
{
    ScreenItem result;
    result.key = this->key;

    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);

    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_enabled_colors(con, element);
    foreground = colors.at(0);
    background = colors.at(1);

    TCODConsole::setColorControl(TCOD_COLCTRL_1, foreground, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::lightGrey+TCODColor::yellow, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lightCyan, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::white, background);

    //key, element, name
    std::stringstream ss;
    ss << this->key << "-" << colfg(foreground, *element);

    if (is_chosen)
    {
        ss << " <-";
    }

    result.foreground = foreground;
    result.background = background;
    result.msg_str = ss.str();
    result.element = element;

    return result;
};

bool MainMenuScreen::is_enabled(std::string* element)
{
    return false;
}

//     template<typename T>
// bool MainMenuScreen<T>::is_active(T* element)
// {
//     return false;
// }
// 
void MainMenuScreen::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char);
    i++;

    char buffer[512];
    std::string msg_str = "%c%s%c";
    sprintf(buffer, msg_str.c_str(), TCOD_COLCTRL_2,
            ((std::string*)si.element)->c_str(), TCOD_COLCTRL_STOP);
    msg_str = buffer;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_str.c_str());
    si.max_y = i;

    i++;
    i++;
};

/* MAIN MENU SCREEN */
ScreenItem SimpleMenuScreen::build_screen_item(TCODConsole* con, int i, MenuItem* element)
{
    ScreenItem result;
    result.key = this->key;

    bool has_duration;
    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);

    std::stringstream ss;
    ss << element->get_title();
    if (is_chosen) { ss << " <-"; };

    std::vector<TCODColor> colors = this->get_enabled_colors(con, element);
    result.foreground = colors.at(0);
    result.background = colors.at(1);
    result.msg_str = ss.str();
    result.element = element;

    return result;
};

bool SimpleMenuScreen::is_enabled(MenuItem* element)
{
    return false;
}

void SimpleMenuScreen::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    std::string msg_char = si.msg_str;
    si.min_y = i;
    con->print(this->offset, i, colfg(si.foreground, msg_char).c_str());
    i++;

    MenuItem* menu_item = static_cast<MenuItem*>(si.element);
    std::string description_line = colfg(TCODColor::lightGrey+TCODColor::yellow, menu_item->get_description());
    con->print(this->offset, i, description_line.c_str());
    si.max_y = i;

    i++;
    i++;
};


/* templates are sorta lame */
template Screen<Item>::Screen();
template bool Screen<Item>::is_chosen(Item* element);
template bool Screen<Item>::is_active(Item* element);
template void Screen<Item>::draw_mouse_horiz_line(TCODConsole* con);
template TCODConsole* Screen<Item>::create_screen();
template void Screen<Item>::draw();
template void Screen<Item>::build_screen_items(TCODConsole* con, int i);
template void Screen<Item>::loop(TCODConsole* con, int i);
template bool Screen<Item>::is_enabled(Item* element);
template ScreenItem Screen<Item>::build_screen_item(TCODConsole* con, int i, Item* element);
template std::vector<TCODColor> Screen<Item>::get_enabled_colors(TCODConsole* con, Item* element);
template void Screen<Item>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);

template Screen<std::string>::Screen();
template bool Screen<std::string>::is_chosen(std::string* element);
template bool Screen<std::string>::is_active(std::string* element);
template void Screen<std::string>::draw_mouse_horiz_line(TCODConsole* con);
template TCODConsole* Screen<std::string>::create_screen();
template void Screen<std::string>::draw();
template void Screen<std::string>::build_screen_items(TCODConsole* con, int i);
template void Screen<std::string>::loop(TCODConsole* con, int i);
template bool Screen<std::string>::is_enabled(std::string* element);
template bool Screen<std::string>::is_active(std::string* element);
template ScreenItem Screen<std::string>::build_screen_item(TCODConsole* con, int i, std::string* element);
template std::vector<TCODColor> Screen<std::string>::get_enabled_colors(TCODConsole* con, std::string* element);
template void Screen<std::string>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);

template Screen<MenuItem>::Screen();
template bool Screen<MenuItem>::is_chosen(MenuItem* element);
template bool Screen<MenuItem>::is_active(MenuItem* element);
template void Screen<MenuItem>::draw_mouse_horiz_line(TCODConsole* con);
template TCODConsole* Screen<MenuItem>::create_screen();
template void Screen<MenuItem>::draw();
template void Screen<MenuItem>::build_screen_items(TCODConsole* con, int i);
template void Screen<MenuItem>::loop(TCODConsole* con, int i);
template bool Screen<MenuItem>::is_enabled(MenuItem* element);
template bool Screen<MenuItem>::is_active(MenuItem* element);
template ScreenItem Screen<MenuItem>::build_screen_item(TCODConsole* con, int i, MenuItem* element);
template std::vector<TCODColor> Screen<MenuItem>::get_enabled_colors(TCODConsole* con, MenuItem* element);
template void Screen<MenuItem>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);


template Screen<Spell>::Screen();
template bool Screen<Spell>::is_chosen(Spell* element);
template bool Screen<Spell>::is_active(Spell* element);
template void Screen<Spell>::draw();
template void Screen<Spell>::build_screen_items(TCODConsole* con, int i);
template void Screen<Spell>::loop(TCODConsole* con, int i);
template ScreenItem Screen<Spell>::build_screen_item(TCODConsole* con, int i, Spell* element);
template void Screen<Spell>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);


template Screen<IClass>::Screen();
template bool Screen<IClass>::is_chosen(IClass* element);
template bool Screen<IClass>::is_active(IClass* element);
template void Screen<IClass>::draw();
template void Screen<IClass>::build_screen_items(TCODConsole* con, int i);
template void Screen<IClass>::loop(TCODConsole* con, int i);
template std::vector<TCODColor> Screen<IClass>::get_enabled_colors(TCODConsole* con, IClass* element);
template ScreenItem Screen<IClass>::build_screen_item(TCODConsole* con, int i, IClass* element);
template void Screen<IClass>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
template bool Screen<IClass>::is_enabled(IClass* element);
