#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <cstdlib>
#include "libtcod.hpp"
#include <exception>
#include <iterator>

#include <jansson.h>

#include "utils.h"
#include "map.h"
#include "Room.h"
#include <actors\Person.h>
#include "game.h"
#include "item.h"
#include "inventory.h"
#include "Representation.h"
#include "tile.h"
#include "enemies\troll.h"
#include "debug_options.h"
#include "messages.h"
#include "ui.h"
#include "combat.h"
#include "attribute.h"
#include "attribute_container.h"
#include <color_utils.h>

Map::Map()
{
    this->enemies = std::vector<Actor*>();
    this->roomVector = new std::vector<Room*>;

    this->has_hero_spawned = false;

    this->width = NULL;
    this->height = NULL;
    this->depth = 1;

}

Map::~Map()
{

}

Tile * Map::getTileAt(int x, int y, bool is_original_pos, int ox, int oy)
{
    std::vector<Tile> * x_tiles;

    if (x < 0  )
    {
        x = 0;
    };

    if ( y < 0  )
    {
        y = 0;
    };

    if (!is_original_pos && (x < 0 || y < 0 ) ) // this shouldnt ever happen now
    { 
        std::cout << "Can't find a tile here.";
        std::cout << "I'm printing to stdout instead of throwing an error." <<std::endl;
    }

    // these two ifs cut way the hell down on recursion by eliminating the
    // need entirely I think
    if (x >= this->width)
    {
        x = this->width-1;
    };

    if (y >= this->height)
    {
        y = this->height-1;
    };


    //try to get the y vector
    try { x_tiles = &tileVector->at(y); }
    catch ( std::out_of_range& ex )
    {
        ex;
        std::cout << "Method getTileAt() recursed on y." << std::endl;
        return getTileAt(x, y-1, false, x, y);
    };

    //try to get the tile from the x vector in the y vector
    // and sense an message if the position isn't the same as the original position
    try 
    {
        auto found_tile = &x_tiles->at(x);
        //auto found_tile2=  &(*tileVector)[y][x]; 
        return found_tile;
    }
    catch ( std::out_of_range& ex )
    {
        ex;
        std::cout << "Method getTileAt() recursed on x." << std::endl;
        return getTileAt(x-1, y, false, x, y);
    };
};

class DungeonListener : public ITCODBspCallback 
{
    private:
        Map& map;
        int roomNum;
        int lastx, lasty;

    public:
        static std::stringstream output;
        DungeonListener(Map &map) : map(map), roomNum(0) {};

        bool visitNode(TCODBsp *node, void *userData)
        {
            if (node->isLeaf())
            {
                // std::cout << "nodes a leaf" << std::endl;

                int room_x, room_y, room_w, room_h;
                int room_min_size = 5;
                // dig a room
                TCODRandom *rng = TCODRandom::getInstance();

                // room_w = rng->getInt(room_min_size, node->w-2);
                // room_h = rng->getInt(room_min_size, node->h-2);
                room_w = rng->getInt(node->w-5, node->w);
                room_h = rng->getInt(node->h-5, node->h);
                room_x = rng->getInt(node->x+1, node->x+node->w-(room_w-1));
                room_y = rng->getInt(node->y+1, node->y+node->h-(room_h-1));

                int perimeter = room_w*2 + room_h*2 - 4;
                // int door_index = rng->getInt(0, perimeter);
                int door_index = -1; //disable doors
                int room_style = rng->getInt(0, 100);
                if (room_style < 75)
                {
                    map.build_rect_room(room_x, room_y, room_w, room_h, door_index);
                }
                else
                {
                    map.build_circle_room(room_x, room_y, room_w, room_h, door_index);
                }

                lastx=room_x+room_w/2;
                lasty=room_y+room_h/2;
                roomNum++;

            }
            else
            {
                DungeonListener::output << "nodes NOT A leaf " << std::endl;
                // std::cout << "nodes NOT A leaf " << std::endl;

                Tile* tile;
                TCODRandom *rng = TCODRandom::getInstance();
                int x, y;
                for (int i = 0; i < 6; i++)
                {
                    x = rng->getInt(node->x+1, node->x+node->w -3);
                    y = rng->getInt(node->y+1, node->y+node->h -3);
                    tile = map.getTileAt(x, y);
                    if ( tile->type_id == TileTypes::FloorTileTypeType)
                    {
                        Representation* stone_repr = new FloorRepresentation;
                        stone_repr->repr = ',';
                        stone_repr->setFGColor(TCODColor::darkerGrey, true, true, true);
                        tile->set_description("A small stone lays here.");
                        tile->set_representation(stone_repr);
                    }
                }
            }
            return true;
        };
};
std::stringstream DungeonListener::output = std::stringstream();

class TownListener : public ITCODBspCallback 
{
    private:
        Map& map;
        int roomNum;
        int lastx, lasty;

    public:
        static std::stringstream output;
        TownListener(Map &map) : map(map), roomNum(0) {};
        bool visitNode(TCODBsp *node, void *userData)
        {
            if (node->isLeaf())
            {
                // std::cout << "nodes a leaf" << std::endl;

                int room_x,room_y,room_w,room_h;
                int room_min_size = 10;
                // dig a room
                TCODRandom *rng = TCODRandom::getInstance();

                room_w = rng->getInt(room_min_size, node->w-2);
                room_h = rng->getInt(room_min_size, node->h-2);
                room_x = rng->getInt(node->x+1, node->x+node->w-(room_w-1));
                room_y = rng->getInt(node->y+1, node->y+node->h-(room_h-1));

                int perimeter = room_w*2 + room_h*2 - 4;
                int door_index = rng->getInt(0, perimeter);
                // int room_style = rng->getInt(0, 100);
                // if (room_style < 75)
                // {
                //     map.build_rect_room(room_x, room_y, room_w, room_h, door_index);
                // }
                // else
                // {
                Room* new_room = this->map.build_circle_room(room_x, room_y, room_w, room_h, door_index);
                int center_x = new_room->center_x, center_y =new_room->center_y+1;
                Person* the_townsmen = Game::create_townsmen("Random Townsmen", 30, center_x, center_y, 't', &this->map);

                //if the Townsmen can path to (0, 0), remove him because the
                //room is not walled in
                the_townsmen->l_path = new TCODPath(this->map.l_map);
                the_townsmen->l_path->compute(center_x, center_y, 0, 0);
                if (the_townsmen->l_path->isEmpty())
                {
                    this->map.enemies.push_back(the_townsmen);
                }
                else
                {
                    delete the_townsmen;
                }


                // }

                lastx=room_x+room_w/2;
                lasty=room_y+room_h/2;
                roomNum++;

                if (roomNum == 6) //stairs only on 6th room
                {
                    Tile* stair_tile = this->map.getTileAt(new_room->center_x, new_room->center_y+1);
                    stair_tile->updateTileType(TileTypes::StairsDownTileTypeType);
                    this->map.l_map->setProperties(stair_tile->tile_x, stair_tile->tile_y, true, true);
                }
            }
            else
            {
                TownListener::output << "nodes NOT A leaf " << std::endl;
                // std::cout << "nodes NOT A leaf " << std::endl;

                Tile* tile;
                TCODRandom *rng = TCODRandom::getInstance();
                int x, y;
                for (int i = 0; i < 16; i++)
                {
                    x = rng->getInt(node->x+1, node->x+node->w -3);
                    y = rng->getInt(node->y+1, node->y+node->h -3);
                    tile = map.getTileAt(x, y);
                    if ( tile->type_id == TileTypes::FloorTileTypeType)
                    {
                        tile->get_representation()->repr = ',';
                        tile->get_representation()->setFGColor(TCODColor::darkerGrey, true, true, true);
                        tile->get_description() = "A small stone presides here.";
                    }
                }
            }
            return true;
        };
};
std::stringstream TownListener::output = std::stringstream();

int Map::build_town_from_random(int seed)
{

    this->width = Game::town_width;
    this->height = Game::town_height;
    this->l_map = new TCODMap(this->width, this->height);
    //the default tile description
    this->description = "This is a town of nice people, please refrain from hurting anyone.";

    this->tileVector = new std::vector<std::vector<Tile>>;
    this->tileVector->resize(this->height);
    for(int ix = 0; ix < this->height; ++ix)
    {
        (*this->tileVector)[ix].resize(this->width);
    }

    int i = 0;
    int x = 0;
    int y = 0;

    int room_min_x = 10;
    int room_min_y = 10;

    while ( i < this->width*this->height )
    {
        Tile* this_tile = getTileAt(x, y);
        this_tile->map = this;
        this_tile->updateTileType(TileTypes::FloorTileTypeType);
        this_tile->get_description() = "Dirt floor.";
        if(this_tile->type_id == TileTypes::FloorTileTypeType)
        {
            //light passes though, walkable
            this->l_map -> setProperties(x, y, true, true);
        }

        else 
        {
            //light does NOT pass through nor is walkable
            this->l_map -> setProperties(x, y, false, false);
        }

        // this_tile->get_description() = "Descriptionless tile.";

        this_tile->tile_x = x;
        this_tile->tile_y = y;

        if ( x >= (this->width -1)  ) // width is 1, only tile would be (0, 0) so you need to substract 1
        {
            y++;
            x = 0;
        }
        else { 
            x++;
        };

        i++;
    }

    TCODBsp bsp(0, 0, this->width, this->height);
    bsp.splitRecursive(NULL, 8, room_min_x, room_min_y, 1.0f, 1.0f);
    TownListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener, this);

    std::cout << "" << TownListener::output.str() << std::endl;

    return 1;

};

int Map::build_dungeon_from_random(int seed, int floor)
{
    //std::string path = get_data_path()+"testing_jansson.json";
    //std::ifstream myfile(path);
    //if (!myfile.is_open())
    //{
    //    throw std::bad_exception();
    //}
    //std::stringstream ss;
    //ss << myfile.rdbuf(); 

    ////JANNSON TESTING
    //json_t* root;
    //json_error_t error;
    //std::string s = ss.str();
    //const char* j_text = s.c_str();
    //root = json_loads(j_text, 0, &error);
    //json_t* text = json_object_get(root, "testing");
    //if (!text)
    //{
    //    std::cout << error.text << std::endl;
    //    throw std::exception("help");
    //}
    //std::cout << json_string_value(text) << std::endl;

    width = Game::map_width;
    height = Game::map_height;
    this->depth = floor;
    l_map = new TCODMap(width, height);
    //the default tile description
    description = "poppycock";

    tileVector = new std::vector<std::vector<Tile>>;
    tileVector->resize(height);
    for(int ix = 0; ix < height; ++ix)
    {
        (*tileVector)[ix].resize(width);
    }

    int i = 0;
    int x = 0;
    int y = 0;

    int room_min_x = 10;
    int room_min_y = 10;

    while ( i < width*height )
    {
        Tile* this_tile = getTileAt(x, y);
        this_tile->map = this;
        this_tile->updateTileType(TileTypes::WallTileTypeType);
        // this_tile->updateTileType(TileTypes::FloorTileTypeType);
        // if(this_tile->type_id == TileTypes::FloorTileTypeType)
        // {
        //     //light passes though, walkable
        //     l_map -> setProperties(x, y, true, true);
        // }
        // else 
        // {
        //     //light does NOT pass through nor is walkable
        //     l_map -> setProperties(x, y, false, false);
        // }

        this_tile->tile_x = x;
        this_tile->tile_y = y;

        if ( x >= (width -1)  ) // width is 1, only tile would be (0, 0) so you need to substract 1
        {
            y++;
            x = 0;
        }
        else { 
            x++;
        };

        i++;
    }

    //split the map into a bsp
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(NULL, 8, room_min_x, room_min_y, 1.0f, 1.0f);

    //go through entire map and build rooms
    DungeonListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener, this);

    //connect rooms to each of their nearest neighbors.

    int distance = 999;
    int new_distance;
    Room* current_room;

    auto v = this->roomVector;
    auto it = v->begin();
    std::vector<Room*> unused_rooms = std::vector<Room*>(*v);
    int room_num = 0;

    for (it; it != v->end(); it++)
    {

        //draw line between *it and current room
        if (std::next(it, 1) == v->end())
        {
            current_room = v->back();
        }
        else
        {
            current_room = *std::next(it, 1);
        }
        TCODLine::init((*it)->center_x, (*it)->center_y, current_room->center_x, current_room->center_y);
        int draw_x=10, draw_y=10;
        do {
            this->getTileAt(draw_x, draw_y)->updateTileType(TileTypes::FloorTileTypeType);
            this->getTileAt(draw_x+1, draw_y)->updateTileType(TileTypes::FloorTileTypeType);
            // this->getTileAt(draw_x, draw_y)->tile->representation->setBGColor(TCODColor::red, true, true, true);
        }
        while (!TCODLine::step(&draw_x, &draw_y));

    };

    //std::cout << "" << BspListener::output.str() << std::endl;
    Room* room = v->at(6);
    Tile* stair_tile = this->getTileAt(room->x+1, room->y+1);
    this->stair_x = stair_tile->tile_x;
    this->stair_y = stair_tile->tile_y;
    stair_tile->updateTileType(TileTypes::StairsDownTileTypeType);
    this->l_map->setProperties(stair_tile->tile_x, stair_tile->tile_y, true, true);

    return 1;

};


Room* Map::build_circle_room(int room_x, int room_y,
        int room_width, int room_height,
        int door_index)
{
    //create and add the room to the list of vectors
    Room * room = new Room(room_x, room_y, room_width, room_height, door_index);
    this->roomVector->push_back(room);

    for(int new_y=0; new_y<room_height; new_y++)
    {
        for(int new_x=0; new_x<room_width;new_x++)
        {
            // std::cout << new_x << " x y " << new_y << std::endl;
            int adj_x = room_x + new_x;
            int adj_y = room_y + new_y;

            //done to save time later on for getTileAt recursion
            if (adj_x >= this->width-1) //adjusting since its 0 index
            {
                // break;
                adj_x = this->width-2;
            }
            if (adj_y >= this->height-1)
            {
                // break;
                adj_y = this->height-2;
            }

            Tile* tile = getTileAt(adj_x, adj_y);
            //check for outer perimeter
            if (room->isCircle(adj_x, adj_y))
            {
                // std::cout << "is circle" << std::endl;
                tile->updateTileType(TileTypes::WallTileTypeType); //for wall
                l_map -> setProperties(adj_x, adj_y, false, false);

                //place door if valid position
                if (room->checkDoorCount())
                {
                    tile->updateTileType(TileTypes::DoorTileTypeType); //for door
                    l_map -> setProperties(adj_x, adj_y, false, false);
                }
            }

            //everything else
            else 
            {
                tile->updateTileType(TileTypes::FloorTileTypeType); //for floor
                //tile->get_representation()->setFGColor(*(tile->get_representation()->fg_color) * 0.5f, true, false, true); //set darker indoor color
                tile->get_description() = "Crumbling bricks are scattered here.";
            }

        }
    }

    return room;
};
Room* Map::build_rect_room(int room_x, int room_y,
        int room_width, int room_height,
        int door_index)
{
    //create and add the room to the list of vectors
    Room * room = new Room(room_x, room_y, room_width, room_height, door_index);
    this->roomVector->push_back(room);

    for(int new_y=0; new_y<room_height; new_y++)
    {
        for(int new_x=0; new_x<room_width;new_x++)
        {
            // std::cout << new_x << " x y " << new_y << std::endl;
            int adj_x = room_x + new_x;
            int adj_y = room_y + new_y;

            //done to save time later on for getTileAt recursion
            if (adj_x >= this->width-1) //adjusting since its 0 index
            {
                // break;
                adj_x = this->width-2;
            }
            if (adj_y >= this->height-1)
            {
                // break;
                adj_y = this->height-2;
            }


            Tile* tile = getTileAt(adj_x, adj_y);
            //check for outer perimeter
            if (room->isPerimeter(new_x, new_y))
            {
                tile->updateTileType(TileTypes::WallTileTypeType); //for wall
                l_map -> setProperties(adj_x, adj_y, false, false);

                //place door if valid position
                if (door_index != -1 && room->checkDoorCount())
                {
                    tile->updateTileType(TileTypes::DoorTileTypeType); //for door
                    l_map -> setProperties(adj_x, adj_y, false, false);
                }
            }
            //everything else
            else 
            {
                tile->updateTileType(TileTypes::FloorTileTypeType); //for floor
                //set darker indoor color
                // tile->get_representation()->setFGColor(*(tile->get_representation()->fg_color) * 0.5f, true, false, true); 
            }

        }
    }

    return room;

};

int Map::build_from_file(std::string filename)
{
    // std::string line;
    // std::ifstream myfile (filename);


    // if (myfile.is_open())
    // {
    //     // get width
    //     getline (myfile,line);
    //     width = atoi(line.c_str());

    //     // get height
    //     getline (myfile,line);
    //     height = atoi(line.c_str());

    //     l_map = new TCODMap(width, height);
    //     // printf("Width: %s, Height: %s\n", width, height);
    //     // std::cout << width << " " << height <<std::endl;

    //     // get default tile description
    //     getline (myfile,line);
    //     description = line;

    //     tileArray = new Tile[width*height];

    //     int i = 0;
    //     int x = 0;
    //     int y = 0;
    //     while (i < width*height) //had to change this from file.good because it was reading too far. Not sure yet.
    //     {
    //         getline (myfile,line);

    //         getline (myfile,line);
    //         int tileType = atoi(line.c_str());
    //         tileArray[i].map = this;
    //         tileArray[i].updateTileType(tileType);

    //         // printf("x %i y %i\n", x, y);
    //         if(tileArray[i].type_id == TileTypes::FloorTileTypeType)
    //         {
    //             //light passes though, walkable
    //             l_map -> setProperties(x, y, true, true);

    //             // printf("see through ");
    //             // printf("this should be true: %s\n", BoolToString(l_map->isWalkable(x, y)));
    //         }

    //         else 
    //         {
    //             l_map -> setProperties(x, y, false, false);
    //             // printf("NOT see through ");
    //             // printf("this should be false: %s\n", BoolToString(l_map->isWalkable(x, y)));
    //         }

    //         if(tileArray[i].type_id == TileTypes::WarpTileTypeType)
    //         {
    //             // WarpTileType* warp_tile;
    //             // warp_tile = (WarpTileType*) tileArray[i].tile;

    //             // getline (myfile,line);
    //             // warp_tile->warpMap = atoi(line.c_str());
    //             // getline (myfile,line);
    //             // warp_tile->warpX = atoi(line.c_str());
    //             // getline (myfile,line);
    //             // warp_tile->warpY = atoi(line.c_str());

    //             // getline (myfile,line);
    //             // warp_tile->description = line;
    //         }
    //         else
    //         {
    //             getline (myfile,line);
    //             tileArray[i].tile->description = line;
    //         }

    //         tileArray[i].tile_x = x;
    //         tileArray[i].tile_y = y;

    //         // printf("x: %i, y: %i\n", x, y);
    //         if ( x >= (width -1)  ) // width is 1, only tile would be (0, 0) so you need to substract 1
    //         {
    //             y++;
    //             x = 0;
    //         }
    //         else 
    //         {
    //             x++;

    //         };

    //         i++;
    //     }

    //     myfile.close();
    // }

    return 1;
}

int Map::draw()
{
    int x,y;

    l_map->computeFov(Game::player->x, Game::player->y, Game::fov_radius, true, FOV_SHADOW);

    for(x=0; x<width;x++)
    {
        for(y=0; y<height; y++)
        {
            Tile * the_tile = getTileAt(x, y);
            TCODColor* the_bg_color = &(TCODColor)(TCODColor::black);
            TCODColor* the_fg_color = &(TCODColor)(TCODColor::white);
            char the_char;

            // std::cout << "initial white and black" <<std::endl;
            // std::cout << the_fg_color <<std::endl << the_bg_color <<std::endl;

                // if (true)
            if (Game::debug_opts->all_vision || l_map->isInFov(x, y))
            {
                the_tile->setKnown(true);

                if(the_tile->is_occupied())
                {
                    //check for item first, so it'll get drawn over by actors
                    if (the_tile->check_for_items())
                    {
                        Item* last_item = the_tile->inventory->items->back();
                        the_char = last_item->repr->repr;
                    };

                    Game::player->mark_as_seen(x, y, the_tile->occupant);

                    the_char = the_tile->occupant->representation->repr;
                    the_fg_color = the_tile->occupant->representation->fg_color;
                    the_bg_color = the_tile->occupant->representation->bg_color;
                }
                else //tile is not occupied
                {
                    the_char = the_tile->get_representation()->repr;

                    TCODColor* tile_temp_col = the_tile->get_representation()->temp_bg_color;
                    TCODColor* tile_orig_col = the_tile->get_representation()->bg_color;

                    Game::game_console->putChar(x, y, the_char);
                    //if the temp colour isnt the original color
                    if ( tile_temp_col != tile_orig_col) {
                        the_bg_color = tile_temp_col;
                        the_tile->get_representation()->temp_bg_color = tile_orig_col;
                    }
                    //if the two colors are the same
                    else 
                    {
                        the_fg_color = the_tile->get_representation()->fg_color;
                    };

                    if (the_tile->check_for_items())
                    {
                        Item* back_item =the_tile->inventory->items->back(); 
                        the_char = back_item->repr->repr;
                        the_fg_color = back_item->repr->fg_color;
                    };
                };

                if (l_map->isTransparent(x, y) == true) 
                {
                }
                else
                {
                };

                Game::game_console->putChar(x, y, the_char);
                Game::game_console->setCharForeground(x, y, *the_fg_color);
                Game::game_console->setCharBackground(x, y, *the_bg_color);


            }
            //tile isn't in FOV
            else 
            {
                if(the_tile->is_occupied())
                {
                    //check for item first, so it'll get drawn over by actors
                    Game::player->mark_as_unseen(the_tile->occupant);

                    the_char = the_tile->occupant->representation->repr;
                    the_fg_color = the_tile->occupant->representation->fg_color;
                    the_bg_color = the_tile->occupant->representation->bg_color;
                }

                if (the_tile->is_known() == true)
                {
                    char the_char = the_tile->get_representation()->repr;
                    Game::game_console->putChar(x, y, the_char);
                    Game::game_console->setCharBackground(x, y, *(the_tile->get_representation()->bg_color) * TCODColor::darkGrey);
                    Game::game_console->setCharForeground(x, y, *(the_tile->get_representation()->fg_color) * TCODColor::darkGrey);
                }
                else
                {
                    Game::game_console->setCharBackground(x, y, TCODColor::black);
                    Game::game_console->setCharForeground(x, y, TCODColor::black);
                }
            };

            //TODO Debug walkable
            //  if (the_tile->map->l_map->isWalkable(the_tile->tile_x, the_tile->tile_y) == false)
            // {
            //              TCODConsole::root->setCharBackground(x, y, TCODColor::amber);
            // }
            // std::cout << "final white and black" <<std::endl;
            // std::cout << the_fg_color << std::endl << the_bg_color << std::endl << std::endl;
        }
    }

    //TODO debug pathing
    // std::vector<Actor*>* ais = Game::player->actors_in_sight;
    // for(std::vector<Actor*>::iterator it = ais->begin(); it != ais->end(); ++it) {
    //     char the_char = (*it)->representation->repr;
    //     TCODConsole::root->putChar((*it)->dest_x, (*it)->dest_y,the_char);
    //     TCODConsole::root->setCharForeground((*it)->dest_x, (*it)->dest_y, TCODColor::darkRed);

    //     if ((*it)->l_path != NULL){
    //         for (int i=0; i < (*it)->l_path->size(); i++ ) {
    //             int path_x,path_y;
    //             (*it)->l_path->get(i,&path_x,&path_y);
    //             TCODConsole::root->putChar(path_x, path_y, 'x');
    //             TCODConsole::root->setCharBackground(path_x, path_y, TCODColor::lightRed);
    //             // printf ("Astar coord : %d %d\n", path_x,path_y );
    //         }
    //     }
    //     else {
    //         std::cout << "its null" << std::endl;
    //     }
    // }

    //may have just shot readability in the head here...
    // std::cout << std::endl << std::endl;
    // std::cout << "Tile Description:" << std::endl;

    // Person  * thePerson = Game::player;
    //BaseTileType * person_tile = tileArray[thePerson->x+(thePerson->y*width)].tile;
    // BaseTileType * person_tile = getTileAt(thePerson->x, thePerson->y)->tile;


    // std::string pers_desc = getTileAt(thePerson->x, thePerson->y)->get_description();
    // std::string tile_description = (pers_desc != "none" ?  pers_desc : description);
    // std::cout << tile_description;
    // std::cout << std::endl << std::endl;

    return 1;
}

bool Map::pos_in_map(int x, int y)
{
    return x < this->width && x >= 0 && y < this->height && y >= 0;
};

bool Map::attackMovePlayer(Person *thePerson, int x2, int y2)
{
    // std::cout << "trying to move player" << std::endl;

    int new_x, new_y; // where the player is intending to go
    new_x = thePerson->x+x2;
    new_y = thePerson->y+y2;

    //check to make sure the target tile's position would be valid
    // if(new_x >= width || new_x < 0 || new_y >= height || new_y < 0)
    if (!this->pos_in_map(new_x, new_y))
    {
        std::cout << "This was an invalid move kind stranger." << std::endl;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "Leave the outer limits alone");
        return false;
    }

    Tile *player_tile; // the current player position
    player_tile = getTileAt(thePerson->x, thePerson->y);

    Tile *target_tile; // the tile of the new position
    target_tile = getTileAt(new_x, new_y);

    int vec[] = {FloorTileTypeType, WarpTileTypeType, StairsDownTileTypeType, StairsUpTileTypeType}; //walkable types of tiles
    std::vector<int> vec_ints(vec, vec+4);
    bool in_types = std::find(vec_ints.begin(), vec_ints.end(), target_tile->type_id)!=vec_ints.end();

    if (this->pos_in_map(new_x, new_y) && in_types && !target_tile->is_occupied())
        // if (new_x < width && new_x > -1 && new_y < height && new_y > -1 &&
        // in_types && !target_tile->is_occupied())
    {
        thePerson->has_attacked = false;
        thePerson->combat->last_victim = NULL;
        thePerson->putPerson(target_tile, new_x, new_y);
        return true;
    }

    //fight if the tile is occupied
    else if (target_tile->is_occupied())
    {
        bool is_fighter = target_tile->occupant->is_fighter;
        if (is_fighter)
        {
            thePerson->has_attacked = true;
            thePerson->attack(target_tile->occupant);
            return false;
        }
        else 
        {
            thePerson->has_attacked = false;
            thePerson->talk_to(target_tile->occupant);
            return true;
        };
    }

    //doors
    else if (target_tile->type_id == TileTypes::DoorTileTypeType)
    {
        if (target_tile->is_open)
        {
            thePerson->putPerson(target_tile, new_x, new_y);
        }
        else
        {
            std::cout << "that door's closed bro." << std::endl;
        };
    }

    else
    {
        std::cout << std::endl << "invalid move." << std::endl;
        new Message(Ui::msg_handler_main, NOTYPE_MSG, "That's probably a wall.");
        // if(new_x < width && new_x > -1 && new_y < height && new_y > -1)
        if (this->pos_in_map(new_x, new_y))
        {
            std::cout << target_tile->get_description() << std::endl;
        }
        else
        {  // more blank space for gui consistency
            std::cout << std::endl;
        }

        return false;
    }
    return false;
}

bool Map::should_spawn_hero()
{
    int result = Game::event_rng->getInt(0, 1000);
    return (this->has_hero_spawned == false && result == SpecialHeroSpawn);
};

void Map::spawn_hero()
{        
    this->has_hero_spawned = true;

    Person* hero;

    if (this->depth == 1)
    {
        //I should eventually move these definitions to serialized files
        //And perhaps use googles newly opensourced super memory efficient
        //Library for reading straight from serialized files with minimal
        //Memory footprint
        Room* room = Game::current_map->roomVector->back();
        hero = Game::create_person("Mighty Elric Tomes", 99, room->center_x, room->center_y, 'E', Game::current_map);
        hero->attrs->health->current_val = 200;
        hero->attrs->health->max_val = 200;
        hero->attrs->armor->current_val = 3;
        hero->attrs->armor->max_val = 3;
        hero->attrs->damage->current_val = 20;
        hero->attrs->damage->max_val = 20;
        hero->xp_value = 200;
        hero->is_hero = true;
        hero->img_path = get_data_path()+"img/hero8x8.png";
        Game::current_map->enemies.push_back(hero);
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkerRed, "ELRIC APPROACHES"));
    }
    else if (this->depth == 2)
    {
        Room* room = Game::current_map->roomVector->back();
        hero = Game::create_person("Spindly Jackson Rook", 99, room->center_x, room->center_y, 'J', Game::current_map);
        hero->attrs->health->current_val = 300;
        hero->attrs->health->max_val = 300;
        hero->attrs->armor->current_val = 10;
        hero->attrs->armor->max_val = 10;
        hero->attrs->damage->current_val = 15;
        hero->attrs->damage->max_val = 15;
        hero->xp_value = 500;
        hero->img_path = get_data_path()+"img/thief8x8.png";
        hero->is_hero = true;
        Game::current_map->enemies.push_back(hero);
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkerRed, "JACKSON ARRIVES"));
    }
    else if (this->depth == 3)
    {
        Room* room = Game::current_map->roomVector->back();
        hero = Game::create_person("Careful John Spiggot", 99, room->center_x, room->center_y, 'J', Game::current_map);
        hero->attrs->health->current_val = 400;
        hero->attrs->health->max_val = 400;
        hero->attrs->armor->current_val = 15;
        hero->attrs->armor->max_val =  15;
        hero->attrs->damage->current_val = 23;
        hero->attrs->damage->max_val = 23;
        Game::current_map->enemies.push_back(hero);
        hero->img_path = get_data_path()+"img/hero8x8.png";
        hero->is_hero = true;
        hero->xp_value = 800;
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkerRed, "JOHN APPEARS"));

    }
    else if (this->depth == 4)
    {
        Room* room = Game::current_map->roomVector->back();
        hero = Game::create_person("Sneaking Samuel Giller", 99, room->center_x, room->center_y, 'S', Game::current_map);
        hero->attrs->health->current_val = 400;
        hero->attrs->health->max_val = 400;
        hero->attrs->armor->current_val = 5;
        hero->attrs->armor->max_val =  5;
        hero->attrs->damage->current_val = 23;
        hero->attrs->damage->max_val = 23;
        Game::current_map->enemies.push_back(hero);
        hero->img_path = get_data_path()+"img/thief8x8.png";
        hero->is_hero = true;
        hero->xp_value = 1000;
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkerRed, "SAMUEL NEARS"));

    }
    else if (this->depth >= 5)
    {
        Room* room = Game::current_map->roomVector->back();
        hero = Game::create_person("Victorious Mr Rossignol", 99, room->center_x, room->center_y, 'R', Game::current_map);
        hero->attrs->health->current_val = 600;
        hero->attrs->health->max_val = 600;
        hero->attrs->armor->current_val = 17;
        hero->attrs->armor->max_val =  17;
        hero->attrs->damage->current_val = 43;
        hero->attrs->damage->max_val = 43;
        hero->xp_value = 1500;
        Game::current_map->enemies.push_back(hero);
        hero->img_path = get_data_path()+"img/hero8x8.png";
        hero->is_hero = true;
        new Message(Ui::msg_handler_main, HELP_MSG, colfg(TCODColor::darkerRed, "ROSSIGNOL ADVANCES"));
    }

    // Game::center_camera_on(hero->my_tile);
    hero->my_tile->setKnown(true);
    std::vector<Tile*>* adjacent_tiles = hero->my_tile->getAdjacentTiles(1);
    std::vector<Tile*>* adjacent_tiles2 = hero->my_tile->getAdjacentTiles(2);
    std::vector<Tile*>* adjacent_tiles3 = hero->my_tile->getAdjacentTiles(3);
    adjacent_tiles->reserve(adjacent_tiles->size()+ adjacent_tiles2->size() + adjacent_tiles3->size());
    adjacent_tiles->insert(adjacent_tiles->end(), adjacent_tiles2->begin(), adjacent_tiles2->end());
    adjacent_tiles->insert(adjacent_tiles->end(), adjacent_tiles3->begin(), adjacent_tiles3->end());
    for (std::vector<Tile*>::iterator it = adjacent_tiles->begin(); it != adjacent_tiles->end(); it++)
    {
        Tile* tile = *it;
        tile->setKnown(true);
    };
    delete adjacent_tiles;
    delete adjacent_tiles2;
    delete adjacent_tiles3;




};

void Map::update()
{
    if (should_spawn_hero())
    {
        spawn_hero();
    };
};
