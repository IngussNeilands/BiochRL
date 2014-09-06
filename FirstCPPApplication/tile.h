#ifndef TILE_H
#define TILE_H

#include <vector>

#include "object.h"
#include "enums\tiletypes_t.h"
//#include "libtcod.hpp"
//#include "../FirstCPPApplication/libtcod_cpp_hpp/libtcod.hpp"
//#include "Representation.h"

class Map;
class Person;
class Actor;
class Object;
class Item;
class Inventory;

class Representation;
class DoorRepresentation;
class WarpRepresentation;
class FloorRepresentation;
class WallRepresentation;
class BaseRepresentation;
class StairsDownRepresentation;
class StairsUpRepresentation;

class TCODColor;
class TCODPath;


class Tile;

class BaseTileType //all tile types must inherit this
{
    public:
        bool collidable;
        Representation * representation;    // the character that represents this tile
        std::string description;      // the description of the tile
        int type_id;
        TCODColor* color;
        Tile* tile_obj;

        BaseTileType();
        ~BaseTileType();


};


class WarpTileType : public BaseTileType
{
    public:
        int warpMap;            // if this is a warp tile, index of the map to warp to
        int warpX, warpY;       // where you warp to in the destination map

        WarpTileType();
};


class WallTileType : public BaseTileType
{
    public: 
        WallTileType();
};


class DoorTileType : public BaseTileType
{
    public: 
        DoorTileType();
        bool is_open;

        // void OpenDoor();
        // void CloseDoor();
        // void ToggleDoor();
};

class FloorTileType : public BaseTileType
{
    public: 
        FloorTileType();
};

class StairsTileType : public BaseTileType
{
    public:
        Map* target_map;
        int to_x, to_y;

        StairsTileType();
        ~StairsTileType();
};

class StairsDownTileType : public StairsTileType
{
    public: 

        bool has_matched_upstair;

        StairsDownTileType();
        void GoDown();
};

class StairsUpTileType : public StairsTileType
{
    public: 
        StairsUpTileType();
        void GoUp();
};

class Tile : public Object
{
    private:
        bool _is_occupied;
        bool _is_known;
        bool is_deleted;


    public:
        BaseTileType* custom_tile;
        BaseTileType* tiletype_obj;

        static BaseTileType* BaseType; //all tile types must inherit this
        static WarpTileType* WarpType;
        static WallTileType* WallType;
        static DoorTileType* DoorType;
        static StairsUpTileType* StairsUpType;
        static StairsDownTileType* StairsDownType;
        static FloorTileType* FloorType;

        Inventory* inventory;

        int type_id;               // type of tile
        BaseTileType* get_tile_type();
        bool is_custom_tile;

        int tile_x;
        int tile_y;

        bool is_open;

        Map* map; //the map that this tile is on

        std::vector<Actor *> * occupants;  // the dude sitting on the tile
        Actor * occupant;

        bool is_occupied();
        bool is_walkable();

        std::string Tile::get_description();
        void Tile::set_description(std::string new_desc);

        Representation*  Tile::get_representation();
        void  Tile::set_representation(Representation* new_repr);

        bool check_for_items();
        void place_item_down(Item* item);
        void pick_up_item(Item* item);

        void OpenDoor();
        void CloseDoor();
        void ToggleDoor();

        bool is_known() { return this->_is_known; };
        void setKnown(bool is_known);

        void drawColorsToRoot(TCODColor* fg_color, TCODColor* bg_color);


        Tile(); 
        //~Tile(){ delete tile; };
        ~Tile();
        Tile(const Tile& other);

        void updateTileType(int type = 0);
        void updateCustomTileType(int type );
        void makeOccupied(Actor* the_actor);
        void makeUnoccupied(Actor* the_actor);

        Tile* getTileAtRelative(int x, int y, int scale=1);
        tile_vec_t* getAdjacentTiles(int scale = 1);
        tile_vec_t* getVacantAdjacentTiles();

        Tile* getTopLeftTile(int scale = 1);
        Tile* getTopMidTile(int scale = 1);
        Tile* getTopRightTile(int scale = 1);
        Tile* getMidRightTile(int scale = 1);
        Tile* getBotRightTile(int scale = 1);
        Tile* getBotMidTile(int scale = 1);
        Tile* getBotLeftTile(int scale = 1);
        Tile* getMidLeftTile(int scale = 1);
};

#endif
