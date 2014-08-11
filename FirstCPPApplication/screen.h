#pragma once
#ifndef SCREEN_H
#define SCREEN_H


class ScreenItem;
class InventoryScreenItem;
class Representation;
class AttrEffect;
class MessageHandler;
class IClass;

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
        std::string get_screen_item_main_text(TCODColor& foreground, TCODColor& background, T* element);
        virtual std::vector<TCODColor> get_colors(TCODConsole* con, T* element);
        virtual std::vector<TCODColor> get_enabled_colors(TCODConsole* con, T* element);
        virtual ScreenItem build_screen_item(TCODConsole* con, int i, T* element) = 0;

        virtual void draw();
        virtual void draw_msg();
        virtual void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si) = 0;


};

class InventoryScreen : public Screen<Item>
{
    public:

        InventoryScreen() : Screen() { this->title="Inventory Screen"; };
        InventoryScreen::~InventoryScreen(){};
        virtual bool is_enabled(Item* element);

        ScreenItem build_screen_item(TCODConsole* con, int i, Item* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

class SpellScreen : public Screen<Spell>
{
    public:

        SpellScreen() : Screen() { this->title="Spell Screen"; };
        SpellScreen::~SpellScreen(){};

        ScreenItem build_screen_item(TCODConsole* con, int i, Spell* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

class ClassScreen : public Screen<IClass>
{
    public:

        ClassScreen() : Screen() { this->title="Choose your class"; };
        ClassScreen::~ClassScreen(){};

        virtual bool is_enabled(IClass* element);
        ScreenItem build_screen_item(TCODConsole* con, int i, IClass* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};

class MainMenuScreen : public Screen<std::string>
{
    public:

        MainMenuScreen() : Screen() { this->title="Main Menu!"; };
        MainMenuScreen::~MainMenuScreen(){};

        virtual bool is_enabled(std::string* element);
        ScreenItem build_screen_item(TCODConsole* con, int i, std::string* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};


class SimpleMenuScreen : public Screen<MenuItem>
{
    public:

        SimpleMenuScreen() : Screen() { this->title="Simple Menu!"; };
        SimpleMenuScreen::~SimpleMenuScreen(){};

        virtual bool is_enabled(MenuItem* element);
        ScreenItem build_screen_item(TCODConsole* con, int i, MenuItem* element);
        void draw_screen_item(TCODConsole* con, int& i, ScreenItem& si);
};


#endif
