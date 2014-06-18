#include "stdafx.h"
#include <sstream>
#include "messages.h"
#include <stdarg.h>
#include <assert.h>
#include <map>

#include <libtcod.hpp>
#include "game.h"
#include "debug_options.h"
#include "utils.h"

//message ordering. started as array and transformed into a vector
message_types_t MessageHandler::initial_message_order [] = {
    HUNGER_MSG, CHAT_MSG,
    HELP_MSG,
    BURDEN_MSG, ITEM_MSG,
    MOOD_MSG, DAMAGE_GIVEN_MSG, DAMAGE_TAKEN_MSG, 
    TILE_DESCRIPTION_MSG, NOTYPE_MSG 
};

typedef MessageHandler MH;
std::vector<message_types_t>
MH::message_order(MH::initial_message_order, MH::initial_message_order + sizeof(MH::initial_message_order) / sizeof(*MH::initial_message_order));

MessageHandler::MessageHandler()
{
    this->msg_list = std::vector<Message*>();
    Message* msg = new Message;
    msg->content = "Welcome to BiochRL++";
    //msg->vlist = " ";
    msg->count = 0;
    this->msg_list.push_back(msg);
    // std::cout << "creating message handler" << std::endl;
};

void MessageHandler::new_msg(Message* message)
{
    //debugging order
    if (Game::debug_opts->show_msg_debug)
    {
        std::stringstream ss;
        //ss <<  "T"<< (int)message->type;
        ss <<  "I"<< Message::getIndex(message->type);
        message->content.append(ss.str());
    };

    Message* last_msg = this->msg_list.back();
    //compare messages to see if the count should increment
    bool same_msg = false;
    if (message->content == last_msg->content)
    {
        same_msg = true;
    }

    if (same_msg && this->msg_list.size() != 0)
        //if (last_msg->content.c_str() == message->content.c_str() && last_msg->vlist == message->vlist && this->msg_list.size() != 0)
    {
        last_msg->count += 1;
        //std::cout << "msg already existed, incrementing count" << std::endl;
    }
    else
    {
        this->msg_list.push_back(message);
    }
}

void MessageHandler::draw_raw(TCODConsole* console)
{
    //go through the latest messages and draw them until there's no more room
    std::vector<Message*>::reverse_iterator it = this->msg_list.rbegin();
    int x = 0;
    int y = 0;
    TCODColor default_color = console->getDefaultForeground();
    for (it; it != this->msg_list.rend(); ++it)
    {
        float coef = ((float)y)/10.0f;
        TCODColor new_color = TCODColor::lerp(default_color, TCODColor::darkGrey, coef);
        console->setDefaultForeground(new_color);
        if ((*it)->count > 0)
        {
            console->print(x, y, ((*it)->content+" (x%d)").c_str(), (*it)->count);
        }
        else 
        {
            console->print(x, y, (*it)->content.c_str());
        };
        y++;

        if (y > 10) break; //don't need to loop over all messages
    };

};

void MessageHandler::draw(TCODConsole* console)
{
    //go through the latest messages and draw them until there's no more room
    std::vector<std::string> string_list = this->PrerenderMessages(10);
    std::vector<std::string>::iterator it = string_list.begin();
    int x = 0;
    int y = 0;
    TCODColor default_color = console->getDefaultForeground();
    for (it; it != string_list.end(); ++it)
    {

        TCODColor  new_color;
        float coef = ((float)y)/10.0f;
        if (y == 0)
        {
            new_color = TCODColor::lighterRed;
        }
        else 
        {
            new_color = TCODColor::lerp(default_color, TCODColor::darkestGrey, coef);
        };
        console->setDefaultForeground(new_color);

        //check width of drawing area and wrap
        int y_height = console->printRect(x, y, console->getWidth(), console->getHeight(), (it)->c_str());
        y += y_height;

        if (y > 10) break; //don't need to loop over all messages
    };

};

int Message::getIndex(message_types_t type)
{
    std::vector<message_types_t> vec = MessageHandler::message_order;
    auto it = std::find(vec.begin(), vec.end(), type);
    if (it == vec.end())
    {
        // message_type not in message_order
        assert(false && "fix missing type");
        return -2;
    } 
    else
    {
        int index = std::distance(vec.begin(), it);
        return index;
    }
}

bool sorting_by_type(Message* a, Message* b)
{
    // std::cout << a->content;
    // std::cout << b->content;
    if (a->turn > b->turn)
    {
        return true;
    }
    else if (a->turn == b->turn)
    {
        return a->type_index < b->type_index;
        // return Message::getIndex(a->type) < Message::getIndex(b->type);
    }
    else if (a->turn < b->turn)
    {
        return false;
    }
    else
    {
        assert(false && "this means turn is not anything rational, I guess.");
    };

};

//goes through all the vectors of this turn and creates a std::string of the
//ones that share a turn
std::vector<std::string> MessageHandler::PrerenderMessages(int turn_limit)
{
    std::vector<std::string> prerendered_msgs;

    double last_turn = Game::turn_count-1;
    double turn_count = 0;
    int copy_count = 2;
    std::string prerendered_single = "";
    std::string last_msg = "";
    std::string new_msg = "";

    std::vector<Message*> limited_messages;
    int message_limit = 200;
    int i = 1;
    for (std::vector<Message*>::reverse_iterator it = this->msg_list.rbegin(); it != this->msg_list.rend(); ++it) {
        limited_messages.push_back((*it));

        if (++i>message_limit) break;
    }

    std::stable_sort(limited_messages.begin(),
            limited_messages.end(),
            sorting_by_type);

    std::map<double, std::vector<std::string>> turn_msgs;


    // for (std::vector<Message*>::reverse_iterator it = this->msg_list.rbegin(); it != this->msg_list.rend(); ++it) {
    last_turn = limited_messages.front()->turn;
    std::string cur_msg;
    std::stringstream ss;
    for (std::vector<Message*>::iterator it = limited_messages.begin(); it != limited_messages.end(); ++it) 
    {
        Message* msg = (*it);
        cur_msg = msg->content;
        if (msg->count > 0) 
        {
            ss << " (x" << msg->count+1 << ")";
            cur_msg.append(ss.str());
            ss.clear();
        }

        if (turn_msgs.find(msg->turn) != turn_msgs.end())
        {
            turn_msgs[msg->turn].push_back(" "+cur_msg);
        }
        else
        {
            //printf("existing\n");
            std::vector<std::string> vec;
            vec.push_back(cur_msg);
            std::pair<double, std::vector<std::string>> msg_pair(msg->turn, vec);
            turn_msgs.insert(msg_pair);
        };




        // if (prerendered_msgs.size() >= turn_limit+1) break;
    }

    typedef std::map<double, std::vector<std::string>>::reverse_iterator it_type;
    for(it_type iterator = turn_msgs.rbegin(); iterator != turn_msgs.rend(); iterator++) 
    {
        for (std::vector<std::string>::iterator it = iterator->second.begin(); it != iterator->second.end(); it++)
        {
            prerendered_single.append(*it);
        }
        //char buffer[1024];
        //std::size_t len = prerendered_single.copy(buffer, prerendered_single.size(), 0);
        //buffer[len] = '\0';
        //prerendered_msgs.push_back(std::string(buffer));
        prerendered_msgs.push_back(prerendered_single);
        prerendered_single.clear();
        // iterator->first = key
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
    }
    // if (prerendered_single.size() > 0) {
    //     prerendered_msgs.push_back(prerendered_single);
    // }

    return prerendered_msgs;
}

void Message::Init()
{
    this->type = message_types_t::NOTYPE_MSG;
    this->type_index = 0;
    this->content = "Unspecified %s";
    this->count = 0;
    this->turn = Game::turn_count;
};

Message::Message()
{
    this->Init();
};

Message::Message(MessageHandler* handler, message_types_t type, std::string content, ...)
{
    assert(content.size()!=0);

    this->Init();
    this->type = type;
    this->type_index = Message::getIndex(type);

    va_list ap;
    va_start(ap, content);

    char* x = new char[512];
    vsprintf(x, content.c_str(), ap);
    this->content = std::string(x);

    va_end(ap);

    handler->new_msg(this);
    this->turn = handler->game->turn_count;
};

