#include "stdafx.h"
#include "gods.h"

#include <game.h>
#include <ui.h>
#include <messages.h>

God::God()
{
    this->name = "The Nameless One";
};


int_str_map_t God::get_favour_threshold(Actor* worshipper)
{
    int_str_map_t thres_map = std::map<int, std::string>();
    thres_map[10]   =  "You pray, and hear a small whisper in the distance.";
    thres_map[25]   =  "You pray, and hear a quiet murmur.";
    thres_map[75]   =  "You pray, and sense a presence near you.";
    thres_map[150]  =  "You pray, and feel the air thicken.";
    thres_map[300]  =  "You pray, and feel a warmth next to you.";
    thres_map[450]  =  "You pray, and hear a voice speak your name.";
    thres_map[600]  =  "You pray, and feel your connection with your god in your body";
    thres_map[1500] =  "You start to pray, but realize there is no need. You are the only god you need.";

    return thres_map;
};

void God::pray_to(Actor* worshipper)
{
    int favour = this->get_favor(worshipper);

    int_str_map_t thres_map = this->get_favour_threshold(worshipper);

    bool satisfied = false;
    std::string msg;
    for (int_str_map_t::reverse_iterator it = thres_map.rbegin(); it!= thres_map.rend(); it++ )
    {
        if (favour > it->first)
        {
            msg = it->second;
            satisfied = true;
            break;
        }
    }
    if (!satisfied)
    {
        msg = "You pray to your god, but you hear no reply.";
    }

    new Message(Ui::msg_handler_main, ITEM_MSG, colfg(TCODColor::lightRed, msg));
};

AugustineGod::AugustineGod() 
{
    this->name = "Augustine";
};

int AugustineGod::get_favor(Actor* worshipper)
{
    return Game::stats->monsters_killed;
};

EpiphneGod::EpiphneGod() 
{
    this->name = "Epiphne";
};

int EpiphneGod::get_favor(Actor* worshipper)
{
    return Game::stats->spells_cast;
};

TheTransientGod::TheTransientGod() 
{
    this->name = "The Transient";
};

int TheTransientGod::get_favor(Actor* worshipper)
{
    return Game::stats->items_used;
};
