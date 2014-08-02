#include "stdafx.h"
#include "screenitem.h"
#include <game.h>
#include <ui.h>
#include "input.h"

ScreenItem::ScreenItem()
{
    this->min_y = NULL;
    this->max_y = NULL;

    this->key = NULL;

    this->foreground = TCODColor::white;
    this->background = TCODColor::black;

    this->msg_str = "Unset ScreenItem string";
};

bool ScreenItem::mouse_is_hovering()
{
    return Game::mouse_evt.cy >= this->min_y && Game::mouse_evt.cy <= this->max_y;
};

void ScreenItem::handle_mouse(int& i)
{
    // printf("min %d max %d\n", min_y, max_y);
    if (Game::mouse_evt.lbutton_pressed)
    {
        if (this->mouse_is_hovering())
        {
            // printf("this name %s", this->msg_str.c_str());
            if ( (this->element)!= Ui::chosen_generic)
            {
                Ui::chosen_generic = (this->element);
                Ui::generic_active = false;
                Input::generic_index = this->key;
            }
            else if ( (this->element) == Ui::chosen_generic)
            {
                Ui::generic_active = true;
                // this->background = TCODColor::green;
            };
        }
    }
    else if (Game::mouse_evt.rbutton_pressed)
    {
        Ui::chosen_generic = NULL;
        Ui::generic_active = false;
    }
    else 
    {
        if (this->mouse_is_hovering())
        {
            this->background = TCODColor::white; //draw happens before mouse
        };
    };


};
