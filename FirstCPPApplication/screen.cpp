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


    template<typename T>
Screen<T>::Screen() 
{
    this->title = "Untitled Screen";

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

    for (int i = 0; i < line_height; i++)
    {
        con->putChar(x, i+offset, '.');
    };

    char bot_char = ' ';
    if ( left_to_draw ) { bot_char = 'v'; }
    con->putChar(x, 25, bot_char);

}

    template<typename T>
void Screen<T>::build_screen_items(TCODConsole* con, int i)
{
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

    template<typename T>
bool InventoryScreen<T>::is_enabled(T* element)
{
    return Ui::game->player->equipment->is_item_equipped(element);
};


    template<typename T>
ScreenItem InventoryScreen<T>::build_screen_item(TCODConsole* con, int i, T* element)
{
    ScreenItem result;
    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_enabled_colors(con,  element);
    foreground = colors.at(0);
    background = colors.at(1);

    std::stringstream ss;
    std::string key = char_to_str(this->key);
    std::string letter = colfg(*element->repr->fg_color, char_to_str(element->repr->repr));
    std::string name = colfg(foreground, element->name);
    std::string weight = colfg(TCODColor::lightGrey, std::string("weighs ")+std::to_string((long double)element->weight)+" stones");
    ss << key << "-" << letter << " " << name << " : " << weight;

    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);
    bool is_enabled = this->is_enabled(element);

    if (is_chosen)
    {
        ss << " <-";
    }
    if (element->custom_key != NULL)
    {
        std::string index = std::to_string((long double)this->get_custom_key_index(element));
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
            if (is_active) { foreground = TCODColor::red+TCODColor::blue; }
            else { foreground = TCODColor::red+TCODColor::green; }
        }
        else { foreground = TCODColor::red;
        };
    }
    else
    {
        background = con->getDefaultBackground();
        if (is_chosen)
        {
            if (is_active) { foreground = TCODColor::red+TCODColor::yellow; }
            else { foreground = TCODColor::red+TCODColor::green; }
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

    template<typename T>
void InventoryScreen<T>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    TCODConsole::setColorControl(TCOD_COLCTRL_1, si.foreground, si.background);
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char);

    i++;

    //print the item effects
    // std::string msg = ((T*)si.element)->attr_effect->oneline_str();
    // std::vector<TCOD_colctrl_t> colctrl_vec = ((T*)si.element)->attr_effect->oneline_str_colours();
    // one_line_helper(con, this->offset, i, msg, colctrl_vec);
    con->print(this->offset, i, ((T*)si.element)->attr_effect->oneline_str_FIXED().c_str());
    si.max_y = i;
    // printf("setting min %d max %d\n", si.min_y, si.max_y);
    i++;
    i++;
};



/* SPELL SCREEN */
    template<typename T>
ScreenItem SpellScreen<T>::build_screen_item(TCODConsole* con, int i, T* element)
{
    ScreenItem result;

    char buffer[512];

    bool has_duration;
    bool is_chosen = this->is_chosen(element);
    bool is_active = this->is_active(element);
    has_duration = element->attr_effect->duration != -1;

    TCODColor foreground, background;
    std::vector<TCODColor> colors = this->get_colors(con, element);
    foreground = colors.at(0);
    background = colors.at(1);

    TCODConsole::setColorControl(TCOD_COLCTRL_1, foreground, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_2, element->get_spell_color(), con->getDefaultBackground());
    TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lightCyan, background);
    TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::white, background);

    //key, element, name
    std::string base_msg_str = "%c-%c%c%c %c%s%c : ";
    sprintf(buffer, base_msg_str.c_str(), this->key, TCOD_COLCTRL_2, 's',
            TCOD_COLCTRL_STOP, TCOD_COLCTRL_1, element->name.c_str(),
            TCOD_COLCTRL_STOP);

    //mana, range
    std::string msg_str = buffer;
    msg_str.append("%c%d mana%c, %c%drng%c");
    sprintf(buffer, msg_str.c_str(), TCOD_COLCTRL_3,
            element->mana_cost, TCOD_COLCTRL_STOP, TCOD_COLCTRL_4,
            element->max_range, TCOD_COLCTRL_STOP);
    msg_str = buffer;

    //duration
    if (has_duration)
    {
        msg_str.append(", %ddur");
        sprintf(buffer, msg_str.c_str(), element->attr_effect->duration);
        msg_str = buffer;
    };

    //aoe
    if (element->aoe > 0)
    {
        std::stringstream ss;
        ss << msg_str << ", " << element->aoe << "aoe";
        msg_str = ss.str();
    };

    if (is_chosen)
    {
        msg_str.append(" <-");
    }

    if (element->custom_key != NULL)
    {
        std::string index = std::to_string((long double)this->get_custom_key_index(element));
        msg_str.append(colfg(TCODColor::green, " "+index));
    };

    result.foreground = foreground;
    result.background = background;
    result.msg_str = msg_str;
    result.element = element;

    return result;
};

    template<typename T>
void SpellScreen<T>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
{
    //print the item name and selection
    const char *msg_char = si.msg_str.c_str();
    si.min_y = i;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_char, this->key );
    i++;

    //print the item effects
    con->print(this->offset, i, ((T*)si.element)->attr_effect->oneline_str_FIXED().c_str());
    si.max_y = i;

    i++;
    i++;
};

/* SPELL SCREEN */
    template<typename T>
ScreenItem ClassScreen<T>::build_screen_item(TCODConsole* con, int i, T* element)
{
    ScreenItem result;

    bool has_duration;
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
    ss << this->key << "-" << colfg(foreground, element->name);

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

    template<typename T>
void ClassScreen<T>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si)
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
            ((T*)si.element)->description.c_str(), TCOD_COLCTRL_STOP);
    msg_str = buffer;
    con->printEx(this->offset, i, TCOD_bkgnd_flag_t::TCOD_BKGND_SET,
            TCOD_alignment_t::TCOD_LEFT, msg_str.c_str());
    si.max_y = i;

    i++;
    i++;
};


    template<typename T>
bool ClassScreen<T>::is_enabled(T* element)
{
    return Ui::game->player->actor_class->type == element->type;
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
template bool InventoryScreen<Item>::is_enabled(Item* element);
template ScreenItem InventoryScreen<Item>::build_screen_item(TCODConsole* con, int i, Item* element);
template std::vector<TCODColor> Screen<Item>::get_enabled_colors(TCODConsole* con, Item* element);
template void InventoryScreen<Item>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);

template Screen<IClass>::Screen();
template bool Screen<IClass>::is_chosen(IClass* element);
template bool Screen<IClass>::is_active(IClass* element);
template void Screen<IClass>::draw();
template void Screen<IClass>::build_screen_items(TCODConsole* con, int i);
template void Screen<IClass>::loop(TCODConsole* con, int i);
template std::vector<TCODColor> Screen<IClass>::get_enabled_colors(TCODConsole* con, IClass* element);
template ScreenItem ClassScreen<IClass>::build_screen_item(TCODConsole* con, int i, IClass* element);
template void ClassScreen<IClass>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
template bool ClassScreen<IClass>::is_enabled(IClass* element);

template Screen<Spell>::Screen();
template bool Screen<Spell>::is_chosen(Spell* element);
template bool Screen<Spell>::is_active(Spell* element);
template void Screen<Spell>::draw();
template void Screen<Spell>::build_screen_items(TCODConsole* con, int i);
template void Screen<Spell>::loop(TCODConsole* con, int i);
template ScreenItem SpellScreen<Spell>::build_screen_item(TCODConsole* con, int i, Spell* element);
template void SpellScreen<Spell>::draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
