#ifndef REPRESENTATION_H
#define REPRESENTATION_H

// #include "../FirstCPPApplication/libtcod_cpp_hpp/libtcod.hpp"


#include "Object.h"

class Object;

class TCODColor;

class Representation : public Object
{
    public:
        char repr;

        //temp fg_color for highlighting etc
         TCODColor* fg_color;
         TCODColor* temp_fg_color;
         TCODColor* orig_fg_color;
                    
         TCODColor* bg_color;
         TCODColor* temp_bg_color;
         TCODColor* orig_bg_color;

        Representation();
        ~Representation();
        Representation(const Representation& source);
        TCODColor* createColorFromRGB(int r, int g, int b);
        void getRGBFromColor(int& r, int& g, int& b, TCODColor color);
        void setFGColor(TCODColor color, bool set_fg, bool set_temp, bool set_orig);
        void setBGColor(TCODColor color, bool set_fg, bool set_temp, bool set_orig);

};


class DeadRepresentation : public Representation
{
    public:
        DeadRepresentation();
};

class BaseRepresentation : public Representation
{
    public:
        BaseRepresentation();

};

class BaseItemRepresentation : public Representation
{
    public:
        BaseItemRepresentation() ;

};

class WarpRepresentation : public Representation
{
    public:
        WarpRepresentation();

};

class WallRepresentation : public Representation
{
    public:
        WallRepresentation();
};

class DoorRepresentation : public Representation
{
    public:
        DoorRepresentation();
};

class ChestRepresentation : public Representation
{
    public:
        ChestRepresentation();
};

class StairsDownRepresentation : public Representation
{
    public:
        StairsDownRepresentation();

};

class StairsUpRepresentation : public Representation
{
    public:
        StairsUpRepresentation();

};

class FloorRepresentation : public Representation
{
    public:
        FloorRepresentation();

};
#endif
