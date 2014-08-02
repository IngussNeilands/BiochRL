#pragma once
#ifndef SCREENITEM_H
#define SCREENITEM_H

class ScreenItem
{
    public:
        int min_y, max_y; //mouse boundaries

        char key;

        TCODColor foreground;
        TCODColor background;
        std::string msg_str;

        void* element;

        void handle_mouse(int& i);
        bool mouse_is_hovering();

        ScreenItem();

};

#endif
