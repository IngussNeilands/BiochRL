#include "stdafx.h"
#include <assert.h>

#include "Room.h"
#include "utils.h"

Room::Room()
{
    this->width = -1;
    this->height = -1;

    this->x = -1;
    this->y = -1;
    this->center_x = -1; 
    this->center_y = -1; 

    this->_door_index_limit = 3;
    this->_current_door_index = 0;
};

Room::Room(int x, int y, int width, int height, int door_index)
{

    this->width = width;
    this->height = height;
    this->inner_w = -1;
    this->inner_h = -1;

    this->x = x;
    this->y = y;

    this->center_x = width/2 + x;
    this->center_y = height/2 + y;
    this->radius = std::min(width/2, height/2)-2;

    this->_door_index_limit = door_index;
    this->_current_door_index = 0;

    assert(x+y+height+width >0 && "make sure room coords make sense");
};

Room::~Room()
{

};

bool Room::checkDoorCount()
{

    this->_current_door_index++;

    if (this->_current_door_index == this->_door_index_limit )
    { 
        return true;
    }
    else
    {
        // std::cout << this->_current_door_index << " limit: " << this->_door_index_limit << std::endl;
        return false;
    }
};

bool Room::isCircle(int point_x, int point_y)
{
    // if (point_x == 0 || point_x == width-1 ||
    //         point_y == 0 || point_y == height-1)
    // {
    //     return true;
    // }
    // return false;

    return is_part_of_circle((double)point_x, (double)point_y,
            (double)this->center_x, (double)this->center_y,
            (double)this->radius);
    //return rad-3 > x + y > rad+3;
};

bool Room::isPerimeter(int point_x, int point_y)
{
    if (point_x == 0 || point_x == width-1 ||
            point_y == 0 || point_y == height-1)
    {
        return true;
    }
    return false;
};

int Room::distanceToRoomCenter(Room* other)
{
    return get_euclidean_distance(this->center_x, this->center_y,other->center_x, other->center_y);
};
