#pragma once
#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

namespace colutl 
{
    std::string fg(TCODColor color, std::string to_wrap)
    {
        char buffer[1024];
        int r = std::max((double)1, (double)color.r);
        int g = std::max((double)1, (double)color.g);
        int b = std::max((double)1, (double)color.b);
        sprintf(buffer, "%c%c%c%c%s%c", TCOD_COLCTRL_FORE_RGB, r, g, b, to_wrap.c_str(), TCOD_COLCTRL_STOP);
        return std::string(buffer);
    };

    std::string bg(TCODColor color, std::string to_wrap)
    {
        char buffer[1024];
        int r = std::max((double)1, (double)color.r);
        int g = std::max((double)1, (double)color.g);
        int b = std::max((double)1, (double)color.b);
        sprintf(buffer, "%c%c%c%c%s%c", TCOD_COLCTRL_BACK_RGB, r, g, b, to_wrap.c_str(), TCOD_COLCTRL_STOP);
        return std::string(buffer);
    };

};

#endif
