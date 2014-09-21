#include "stdafx.h"
#include "Representation.h"


#include "../FirstCPPApplication/libtcod_cpp_hpp/libtcod.hpp"

Representation::~Representation()
{
    delete this->fg_color;
    delete this->temp_fg_color;
    delete this->orig_fg_color;

    delete this->bg_color;
    delete this->temp_bg_color;
    delete this->orig_bg_color;
};

Representation::Representation()
{
    repr = '/';

    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::white);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);

    this->getRGBFromColor(r, g, b, TCODColor::black);
    bg_color = this->createColorFromRGB(r, g, b);
    temp_bg_color = this->createColorFromRGB(r, g, b);
    orig_bg_color = this->createColorFromRGB(r, g, b);
};


void Representation::getRGBFromColor(int& r, int& g, int& b, TCODColor color)
{
    r = color.r;
    g = color.g;
    b = color.b;
};

Representation::Representation(const Representation& source)
{
        this->repr = source.repr;

        //temp fg_color for highlighting etc
         this->fg_color = source.fg_color;
         this->temp_fg_color = source.temp_fg_color;
         this->orig_fg_color = source.orig_fg_color;
                    
         this->bg_color = source.bg_color;
         this->temp_bg_color = source.temp_bg_color;
         this->orig_bg_color = source.orig_bg_color;

};

TCODColor* Representation::createColorFromRGB(int r, int g, int b)
{
    TCODColor* myColor = new TCODColor(r, g, b);
    return myColor;

};

void Representation::setBGColor(TCODColor color, bool set_bg, bool set_temp, bool set_orig)
{
    int r,b,g;
    this->getRGBFromColor(r, g, b, color);
    if (set_bg)
    {
        this->bg_color = this->createColorFromRGB(r, g, b);
    };
    if (set_temp)
    {
        this->temp_bg_color = this->createColorFromRGB(r, g, b);
    };
    if (set_orig)
    {
        this->orig_bg_color = this->createColorFromRGB(r, g, b);
    };
    // std::cout << "rbg: " << r << g << b << std::endl;

};

void Representation::setFGColor(TCODColor color, bool set_fg, bool set_temp, bool set_orig)
{
    int r,b,g;
    this->getRGBFromColor(r, g, b, color);
    if (set_fg)
    {
        this->fg_color = this->createColorFromRGB(r, g, b);
    };
    if (set_temp)
    {
        this->temp_fg_color = this->createColorFromRGB(r, g, b);
    };
    if (set_orig)
    {
        this->orig_fg_color = this->createColorFromRGB(r, g, b);
    };
    // std::cout << "rbg: " << r << g << b << std::endl;

};

DeadRepresentation::DeadRepresentation(): Representation()
{
    repr = 'X';
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::lightRed);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

BaseRepresentation::BaseRepresentation() : Representation()
{
    repr='b';
    //// fg_color = &(TCODColor)(TCODColor::white);
    // bg_color = &(TCODColor)(TCODColor::grey);
    // temp_bg_color = &(TCODColor)(TCODColor::grey);
    // orig_bg_color = &(TCODColor)(TCODColor::grey);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::grey);
    bg_color = this->createColorFromRGB(r, g, b);
    temp_bg_color = this->createColorFromRGB(r, g, b);
    orig_bg_color = this->createColorFromRGB(r, g, b);
};

BaseItemRepresentation::BaseItemRepresentation() : Representation()
{
    repr='?';
    //// fg_color = &(TCODColor)(TCODColor::white);
    // bg_color = &(TCODColor)(TCODColor::grey);
    // temp_bg_color = &(TCODColor)(TCODColor::grey);
    // orig_bg_color = &(TCODColor)(TCODColor::grey);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::grey);
    bg_color = this->createColorFromRGB(r, g, b);
    temp_bg_color = this->createColorFromRGB(r, g, b);
    orig_bg_color = this->createColorFromRGB(r, g, b);
};

WarpRepresentation::WarpRepresentation() : Representation()
{
    repr = 'w';
    // fg_color = &(TCODColor)(TCODColor::sepia);
    // temp_fg_color = &(TCODColor)(TCODColor::sepia);
    // orig_fg_color = &(TCODColor)(TCODColor::sepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::sepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

WallRepresentation::WallRepresentation(): Representation()
{
    repr = '#';
    // fg_color = &(TCODColor)(TCODColor::sepia);
    // temp_fg_color = &(TCODColor)(TCODColor::sepia);
    // orig_fg_color = &(TCODColor)(TCODColor::sepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::sepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

DoorRepresentation::DoorRepresentation(): Representation()
{
    repr = '+';
    // fg_color = &(TCODColor)(TCODColor::darkerSepia);
    // temp_fg_color = &(TCODColor)(TCODColor::darkerSepia);
    // orig_fg_color = &(TCODColor)(TCODColor::darkerSepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::darkerSepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

ChestRepresentation::ChestRepresentation(): Representation()
{
    repr = '*';
    // fg_color = &(TCODColor)(TCODColor::darkerSepia);
    // temp_fg_color = &(TCODColor)(TCODColor::darkerSepia);
    // orig_fg_color = &(TCODColor)(TCODColor::darkerSepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::darkestSepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

StairsDownRepresentation::StairsDownRepresentation(): Representation()
{
    repr = '>';
    // fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // temp_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // orig_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::lighterSepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

StairsUpRepresentation::StairsUpRepresentation(): Representation()
{
    repr = '<';
    // fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // temp_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // orig_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::lighterSepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};

FloorRepresentation::FloorRepresentation(): Representation()
{
    repr = '.';
    // fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // temp_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    // orig_fg_color = &static_cast<TCODColor>(TCODColor::lighterSepia);
    int r,b,g;
    this->getRGBFromColor(r, g, b, TCODColor::lighterSepia);
    fg_color = this->createColorFromRGB(r, g, b);
    temp_fg_color = this->createColorFromRGB(r, g, b);
    orig_fg_color = this->createColorFromRGB(r, g, b);
};
