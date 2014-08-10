#include "stdafx.h"
#include "menu_item.h"

MenuItem::MenuItem()
{
    this->init();
};

void MenuItem::init()
{
    this->title = "Untitled";
    this->description = "No description";

    this->left_box_text = NULL;
    this->mid_box_text = NULL;
    this->right_box_text = NULL;

};

int MenuItem::get_windows_num()
{
    int result;

    if (this->left_box_text != NULL)
    {
        result++;
    };
    if (this->mid_box_text != NULL)
    {
        result++;
    };
    if (this->right_box_text != NULL)
    {
        result++;
    };

    return result;

};
