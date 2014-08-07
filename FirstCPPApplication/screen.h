#pragma once
#ifndef SCREEN_H
#define SCREEN_H


class ScreenItem;
class InventoryScreenItem;
class Representation;
class AttrEffect;
class MessageHandler;

template< typename T>
class Screen 
{
    public: 
        std::string title;
        std::string empty_msg;

        int offset;
        char key;
        int hline;

        // MessageHandler* msg_hdlr;

        std::vector<T*>* elements;
        std::vector<ScreenItem*>* screen_items;

        virtual bool is_chosen(T* element);
        virtual bool is_active(T* element);
        virtual bool is_enabled(T* element) { return false; };

        int get_custom_key_index(T* element);

        Screen();

        TCODConsole* create_screen();

        void draw_empty_msg(TCODConsole* con, int i);
        void draw_screen_title(TCODConsole* con);
        void draw_mouse_horiz_line(TCODConsole* con);
        virtual void loop(TCODConsole* con, int i);
        void Screen<T>::draw_scroll_arrows(TCODConsole* con, bool left_to_draw);

        void build_screen_items(TCODConsole* con, int i);
        virtual std::vector<TCODColor> get_colors(TCODConsole* con, T* element);
        virtual std::vector<TCODColor> get_enabled_colors(TCODConsole* con, T* element);
        virtual ScreenItem build_screen_item(TCODConsole* con, int i, T* element) = 0;

        virtual void draw();
        virtual void draw_msg();
        virtual void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si) = 0;


};

template<typename T>
class InventoryScreen : public Screen<T>
{
    public:

        InventoryScreen<T>() : Screen() { this->title="Inventory Screen"; };
        InventoryScreen::~InventoryScreen(){};
        virtual bool is_enabled(T* element);

        ScreenItem build_screen_item(TCODConsole* con, int i, T* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

template<typename T>
class SpellScreen : public Screen<T>
{
    public:

        SpellScreen<T>() : Screen() { this->title="Spell Screen"; };
        SpellScreen::~SpellScreen(){};

        ScreenItem build_screen_item(TCODConsole* con, int i, T* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

template<typename T>
class ClassScreen : public Screen<T>
{
    public:

        ClassScreen<T>() : Screen() { this->title="Choose your class"; };
        ClassScreen::~ClassScreen(){};

        virtual bool is_enabled(T* element);
        ScreenItem build_screen_item(TCODConsole* con, int i, T* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

template<typename T>
class MainMenuScreen : public Screen<T>
{
    public:

        MainMenuScreen<T>() : Screen() { this->title="Main Menu!"; };
        MainMenuScreen::~MainMenuScreen(){};

        virtual bool is_enabled(T* element);
        // virtual bool is_active(T* element);
        ScreenItem build_screen_item(TCODConsole* con, int i, T* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};


#endif
