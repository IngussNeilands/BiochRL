#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include <vector>
#include <libtcod.hpp>

class Message;
class Game;

enum message_types_t {
    NOTYPE_MSG = 0,
    MOOD_MSG = 1,
    TILE_DESCRIPTION_MSG = 2,
    DAMAGE_GIVEN_MSG = 3, DAMAGE_TAKEN_MSG = 4,
    HELP_MSG = 5, CHAT_MSG = 6,
    HUNGER_MSG = 7, BURDEN_MSG = 8,
    ITEM_MSG = 9
};

class MessageHandler
{
    private:
        static message_types_t initial_message_order [];

    public:
        static std::vector<message_types_t> message_order;
        Game* game;
        std::vector<Message*> msg_list;
        void draw(TCODConsole* console);
        void draw_raw(TCODConsole* console);
        void new_msg(Message* message);
        MessageHandler();
        std::vector<std::string> PrerenderMessages(int turn_limit);


};

class Message
{
    public:
        std::string content;
        int count;
        double long turn;

        message_types_t type;
        int type_index;

        Message();
        Message(MessageHandler* handler, message_types_t type, std::string content, ...);
        void Init();

        static int getIndex(message_types_t type);


};

#endif
