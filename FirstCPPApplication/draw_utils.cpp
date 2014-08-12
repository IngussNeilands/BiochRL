#include "stdafx.h"


#include "libtcod.hpp"
#include "draw_utils.h"


void draw_rect(TCODConsole* con, int x, int y, int w, int h,
        std::string top_c, std::string right_c, std::string bot_c, std::string left_c,
        std::string tr_c, std::string br_c, std::string bl_c, std::string tl_c )
{
    //draw outline
    //top
    for (int i=x; i!=w; i++)
    {
        con->print(i, y, top_c.c_str());
    }
    //right
    for (int i=y; i!=h; i++)
    {
        con->print(x+w-1, i, right_c.c_str());
    }
    //bottom
    for (int i=x; i!=w; i++)
    {
        con->print(i, y+h-1, bot_c.c_str());
    }
    //left
    for (int i=y; i!=h; i++)
    {
        con->print(x, i, left_c.c_str());
    }

    //draw corners
    //top left
    con->print(x, y, tl_c.c_str());
    //top right
    con->print(x+w-1, y, tr_c.c_str());
    //bottom right
    con->print(x+w-1, y+h-1, br_c.c_str());
    //left
    con->print(x, y+h-1, bl_c.c_str());

};
