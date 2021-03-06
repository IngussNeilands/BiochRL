#include "stdafx.h"
#include "helpbox.h"
#include "draw_utils.h"
#include "tile.h"
#include "actors\actor.h"
#include <game.h>
#include "ui.h"

int HelpBox::count = 0;

void HelpBox::initialize()
{
    this->id = HelpBox::count;
    HelpBox::count++;

    int x = Game::camera_w/2, y = Game::camera_h/2;
    this->x = x;
    this->y = y;

    this->left_pad=1;
    this->right_pad=2;
    this->top_pad=1;
    this->bot_pad=2;
    this->con = NULL;
    this->selected_tile = NULL;

    this->return_screen = NoScreenType;

    this->extra_padding = 20;
};

bool HelpBox::operator==(HelpBox& other)
{
    return this->id == other.id;
};

HelpBox::HelpBox()
{
    this->initialize();
};

HelpBox::~HelpBox()
{
    if (this->con != NULL)
    {
        delete this->con;
    }
};

HelpBox::HelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile) 
{
    this->initialize();
    this->selected_tile = selected_tile;
    if (selected_tile == NULL) { this->extra_padding = 0; };
    this->target_con = target_con;
    this->messages = messages;
    int max_width = 17;
    int line_width = 0;
    int line_height = messages.size()*2;
    // printf("%d\n", max_height);

    std::vector<std::string>::iterator it = messages.begin();
    for (it; it!= messages.end(); it++)
    {
        int bigger_width = (int)std::max((double)(*it).size(), (double)line_width); 
        line_width = (int)std::min((double)bigger_width, (double)max_width);
        int height = TCODConsole::root->getHeightRect(0, 0, line_width, 1000, (*it).c_str());
        if (height>1) line_height++;

    };

    this->width = line_width+left_pad+right_pad;
    this->height = line_height+top_pad+bot_pad;
    this->con = new TCODConsole(this->width, this->height+this->extra_padding);
};

void HelpBox::resize(int width, int height)
{
    this->width = width;
    this->height = height;
    delete this->con;
    this->con = new TCODConsole(width, height+this->extra_padding);
};

void HelpBox::set_background()
{
    if (this->selected_tile && !selected_tile->is_known())
    {
        this->con->setDefaultForeground(TCODColor::darkestGrey);
    };
};

void HelpBox::draw_outline()
{
    if (selected_tile && !selected_tile->is_known())
        draw_rect(this->con, 0, 0, this->width, this->height, "?", "?", "?", "?", "?", "?", "?", "?");
    else
        draw_rect(this->con, 0, 0, this->width, this->height);
};

void HelpBox::draw_image()
{

    if (selected_tile && selected_tile->occupant != NULL && selected_tile->is_known())
    {
        TCODImage* img = selected_tile->occupant->get_image();
        int w, h;
        img->getSize(&w, &h);
        if (w == 20)
        {
            img->blit2x(this->con, 4, 9);
        }
        else
        {
            img->blit(this->con, 9, 15);
        }
        delete img;
    };

};

void HelpBox::draw_messages()
{
    int y = 1;
    std::vector<std::string>::iterator it = messages.begin();
    for (it; it!= messages.end(); it++)
    {
        // this->con->print(1+this->left_pad, y+this->top_pad, (*it).c_str());
        int diff = this->con->printRect(1+this->left_pad, y+this->top_pad, this->width-this->left_pad-this->right_pad, this->height, (*it).c_str());
        y+= diff;
        // y++;
        y++;
    };

};

void HelpBox::draw(int& first_x, int& first_y)
{
    int old_x = this->x;
    int old_y = this->y;

    this->x = first_x;
    this->y = first_y;
    this->draw();

    this->x = old_x;
    this->y = old_y;

};

void HelpBox::draw()
{
    this->draw_messages();
    this->set_background();
    this->draw_outline();
    this->draw_image();

    int draw_x = this->x;
    int draw_y = this->y;

    //hack to avoid moving the ui sidebar alerts
    if (this->target_con == Game::game_console)
    {
        draw_x+=Game::camera_x;
        draw_y+=Game::camera_y;
    };

    TCODConsole::root->blit(this->con, 0, 0, this->width+this->extra_padding,
            this->height+this->extra_padding, this->target_con, draw_x,
            draw_y);

};

TargetHelpBox::TargetHelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile) : HelpBox(messages, target_con, selected_tile)
{

};

const void DialogHelpBox::default_accept()
{
    std::cout << "Woo hoo, you love me!" << std::endl;
};

const void DialogHelpBox::default_cancel()
{
    Game::current_screen = this->return_screen;
    delete this;
};


DialogHelpBox::DialogHelpBox(std::vector<std::string> messages, void (*on_accept)(DialogHelpBox* dialog), void (*on_cancel)(DialogHelpBox* dialog), TCODConsole* target_con) : HelpBox(messages, target_con, NULL)
{
    this->on_accept = on_accept;
    this->on_cancel = on_cancel;
};

DialogHelpBox::DialogHelpBox(std::vector<std::string> messages, TCODConsole* target_con) : HelpBox(messages, target_con, NULL)
{
    this->on_accept = NULL;
    this->on_cancel = NULL;
};

DialogHelpBox::~DialogHelpBox()
{
    //delete messages
    this->messages.clear();

    //remove from vectors if its in any
    auto that = this;
    Ui::alerts.erase(
            std::remove_if(
                Ui::alerts.begin(),
                Ui::alerts.end(),
                [this](DialogHelpBox* s) -> bool {return *s == *this;}),
            Ui::alerts.end());
};

void DialogHelpBox::accept()
{
    if (this->on_accept != NULL)
    {
        this->on_accept(this);
    }
    else
    {
        DialogHelpBox::default_accept();
    };
};

void DialogHelpBox::cancel()
{
    if (this->on_cancel != NULL)
    {
        this->on_cancel(this);
    }
    else
    {
        DialogHelpBox::default_cancel();
    };
};
