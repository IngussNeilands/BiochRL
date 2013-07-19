#ifndef TILE_H
#define TILE_H

#include <vector>

#include "libtcod.hpp"

#include "person.h"
#include "Representation.h"



class BaseTileType //all tile types must inherit this
{
    public:
        bool collidable;
        Representation * representation;    // the character that represents this tile
        std::string description;      // the description of the tile
        static int tiletype;
        TCODColor color;

        BaseTileType()
        {
            representation = new BaseRepresentation;
        };

};


class WarpTileType : public BaseTileType
{
    public:
        int warpMap;            // if this is a warp tile, index of the map to warp to
        int warpX, warpY;       // where you warp to in the destination map

        WarpTileType()
        {
            representation = new WarpRepresentation;
        };
};


class WallTileType : public BaseTileType
{
    public: 
        WallTileType()
        {
            representation = new WallRepresentation;
        };
};


class FloorTileType : public BaseTileType
{
    public: 
        FloorTileType(){
            tiletype;
            representation = new FloorRepresentation;
        };
};

class Tile : public Object
{
    bool _is_occupied;

    public:
    BaseTileType * tile;
    int tiletype;               // type of tile

    std::vector<Actor *> occupants;       // the dude sitting on the tile
    Actor * occupant;

    bool is_occupied() { return _is_occupied; };


    Tile(); 
    ~Tile(){ delete tile; };

    void updateTileType(int type = 0);
    void makeOccupied();
    void makeUnoccupied();
};

#endif