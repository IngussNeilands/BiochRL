#ifndef ROOM_H
#define ROOM_H

// #include "map.h"

class Room
{
    public:
        Room();
        ~Room();
        Room(int x, int y, int width, int height, int door_number=3);

        //w and h of the room
        int width, height;

        //its location on the map
        int x, y;
        int center_x, center_y;
        int radius;

        //inner room
        int inner_w, inner_h;

        bool isPerimeter(int point_x, int point_y);
        bool isCircle(int point_x, int point_y);
        bool checkDoorCount();

        int distanceToRoomCenter(Room* other);

    private:
        int _door_index_limit;
        int _current_door_index;


};

#endif
