#include "stdafx.h"
#include "helpbox.h"
#include "draw_utils.h"
#include "tile.h"
#include "actors\actor.h"

void HelpBox::initialize()
{
    this->x = -1;
    this->y = -1;

    this->left_pad=1;
    this->right_pad=2;
    this->top_pad=1;
    this->bot_pad=2;
    this->con = NULL;
    this->selected_tile = NULL;

    this->extra_padding = 20;
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
    this->draw_messages();
    this->set_background();
    this->draw_outline();
    this->draw_image();

    TCODConsole::root->blit(this->con, 0, 0, this->width+this->extra_padding,
            this->height+this->extra_padding, this->target_con, first_x,
            first_y);

};

void HelpBox::draw()
{
    this->draw_messages();
    this->set_background();
    this->draw_outline();
    this->draw_image();

    TCODConsole::root->blit(this->con, 0, 0, this->width+this->extra_padding,
            this->height+this->extra_padding, this->target_con, this->x,
            this->y);

};

TargetHelpBox::TargetHelpBox(std::vector<std::string> messages, TCODConsole* target_con, Tile* selected_tile) : HelpBox(messages, target_con, selected_tile)
{

};

