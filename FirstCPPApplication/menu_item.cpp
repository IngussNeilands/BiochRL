#include "stdafx.h"
#include "menu_item.h"
#include "background_info.h"

MenuItem::MenuItem()
{
    this->init();
};

void MenuItem::init()
{
    this->info = NULL;
};

std::string MenuItem::get_title()
{
    if (this->info != NULL) 
    {
        return this->info->title;
    }
    else
    {
        return "no info found";
    }
};

std::string MenuItem::get_description()
{ 
    if (this->info != NULL) 
    {
        return this->info->description;
    }
    else
    {
        return "no info found";
    }
};

void MenuItem::activate()
{
    this->info->activate();
    std::cout << "menu item activated" << std::endl;
};

void MenuItem::examine()
{
    if (this->info == NULL)
    {
        std::cout << "no matching background info found" << std::endl;
        return;
    }
    this->info->examine();
    std::cout << "menu item examined" << std::endl;
};
