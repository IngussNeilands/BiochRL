#include "stdafx.h"
#include "menu_item.h"
#include "background_info.h"

MenuItem::MenuItem()
{
    this->init();
};

void MenuItem::init()
{
    this->title = "Untitled";
    this->description = "No description";
    this->info = NULL;
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
