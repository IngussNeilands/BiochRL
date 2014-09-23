#include "stdafx.h"
#include "gods.h"

#include <game.h>
#include <ui.h>
#include <messages.h>

God::God()
{
    this->name = "Augustine";
};

void God::pray_to(Actor* worshipper)
{
    int killed = Game::stats->monsters_killed;
    typedef std::map<int, std::string> thres_map_t; 

    thres_map_t kill_map = std::map<int, std::string>();
    kill_map[10]   =  "You pray, and hear a small whisper in the distance.";
    kill_map[25]   =  "You pray, and hear a quiet murmur.";
    kill_map[75]   =  "You pray, and sense a presence near you.";
    kill_map[150]  =  "You pray, and feel the air thicken.";
    kill_map[300]  =  "You pray, and feel a warmth next to you.";
    kill_map[450]  =  "You pray, and hear a voice speak your name.";
    kill_map[600]  =  "You pray, and feel your connection with your god in your body";
    kill_map[1000] =  "You start to pray, but realize there is no need. You are the only god you need.";

    bool satisfied = false;
    std::string msg;
    for (thres_map_t::reverse_iterator it = kill_map.rbegin(); it!= kill_map.rend(); it++ )
    {
        if (killed > it->first)
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

