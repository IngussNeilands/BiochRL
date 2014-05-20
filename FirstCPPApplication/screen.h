#pragma once
#ifndef SCREEN_H
#define SCREEN_H


class ScreenItem;
class InventoryScreenItem;
class Representation;
class ItemEffect;

template< typename T>
class Screen 
{
    public: 
        std::string title;

        int offset;
        char key;
        int hline;

        std::vector<T*>* elements;
        std::vector<ScreenItem*>* screen_items;


        Screen();

        TCODConsole* create_screen();

        void draw_screen_title(TCODConsole* con);
        void draw_mouse_horiz_line(TCODConsole* con);
        virtual void loop(TCODConsole* con, int i) = 0;
        virtual void build_screen_items(TCODConsole* con, int i) = 0;

        virtual void draw();

};

template<typename T>
class InventoryScreen : public Screen<T>
{
    public:

        InventoryScreen<T>() : Screen() { this->title="Inventory Screen"; };
        InventoryScreen::~InventoryScreen(){};
        void build_screen_items(TCODConsole* con, int i);
        void loop(TCODConsole* con, int i);

        ScreenItem build_screen_item(TCODConsole* con, int i, T* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);

            // void draw();
};

class ScreenItem
{
    public:
        int min_y, max_y; //mouse boundaries

        TCODColor foreground;
        TCODColor background;
        std::string msg_str;

        void* element;

        void handle_mouse(int& i);
        bool mouse_is_hovering();

        ScreenItem();

};

#endif
