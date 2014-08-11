#include "stdafx.h"
#include "background_info.h"
#include <game.h>
#include "ui.h"
#include <helpbox.h>

BackgroundInfo::BackgroundInfo()
{
    this->left_box_text = NULL;
    this->mid_box_text = NULL;
    this->right_box_text = NULL;
};

void BackgroundInfo::activate()
{
    std::cout << "background info activated" << std::endl;
};

std::vector<DialogHelpBox*>* BackgroundInfo::create_alerts()
{
    std::vector<DialogHelpBox*>* result = new std::vector<DialogHelpBox*>();
    if (this->left_box_text != NULL)
    {
        DialogHelpBox* left_dialog = new DialogHelpBox(*this->left_box_text, TCODConsole::root);
        left_dialog->return_screen = Game::current_screen;
        result->push_back(left_dialog);
        Ui::alerts.push_back(left_dialog);
    };
    if (this->mid_box_text != NULL)
    {
        DialogHelpBox* mid_dialog = new DialogHelpBox(*this->mid_box_text, TCODConsole::root);
        mid_dialog->return_screen = Game::current_screen;
        result->push_back(mid_dialog);
        Ui::alerts.push_back(mid_dialog);
    };
    if (this->right_box_text != NULL)
    {
        DialogHelpBox* right_dialog = new DialogHelpBox(*this->right_box_text, TCODConsole::root);
        right_dialog->return_screen = Game::current_screen;
        result->push_back(right_dialog);
        Ui::alerts.push_back(right_dialog);
    };

    return result;

};

void BackgroundInfo::position_alerts(std::vector<DialogHelpBox*>* alerts)
{
    int num_alerts = alerts->size();
    if (num_alerts == 1)
    {
        this->position_1_alerts(alerts);
    }
    else
    {
        std::cout << "un supported amount of alerts: " << num_alerts << std::endl;
    }
};

void BackgroundInfo::position_1_alerts(std::vector<DialogHelpBox*>* alerts)
{
    DialogHelpBox* left_dialog = alerts->at(0);
    left_dialog->return_screen = Game::current_screen;
    left_dialog->y = 5;
    //centered
    left_dialog->x = Game::screen_w/2 - (left_dialog->width/2) - left_dialog->left_pad-left_dialog->right_pad;
    left_dialog->resize(left_dialog->width+10, left_dialog->height);
};

void BackgroundInfo::examine()
{
    std::cout << "background info examined" << std::endl;

    //position alerts
    auto alerts = this->create_alerts();
    this->position_alerts(alerts);
    Game::current_screen = Screens::AlertScreenType;

};

int BackgroundInfo::get_windows_num()
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
