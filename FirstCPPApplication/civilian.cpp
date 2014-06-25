#include "stdafx.h"
#include "civilian.h"


#include "libtcod_cpp_hpp\libtcod.hpp"


Civilian::Civilian()
{
    this->disposition = 50;
    this->master = NULL;
    this->chat_lines = new std::vector<std::string>();
    this->chat_lines->push_back("Nice day outside, isn't it? I wouldn't actually know.");
    this->chat_lines->push_back("I smell peanut butter.");
    this->chat_lines->push_back("You know what, I don't care for you at all.");
    this->chat_lines->push_back("If you had any sense at all, you'd be happy to let me touch you.");
    this->chat_lines->push_back("I'm pretty sure I'm trapped in here.");
    this->chat_lines->push_back("When will the forest speak?"); //S
    this->chat_lines->push_back("Listen to the man, he must have a plan."); //S
    this->chat_lines->push_back("Show me and make me a smile I can wear."); //S
    this->chat_lines->push_back("Brotherhood is nothing without your brothers."); //WCAR
    this->chat_lines->push_back("Let my words be your words, let my thoughts be your thoughts."); //ABR
    this->chat_lines->push_back("Run in circles to show your affection to the Myld God");
    this->chat_lines->push_back("Take care not to say too much, lest you anger the Dragon.");
    this->chat_lines->push_back("If you are me and I am you, who is he?");
    this->chat_lines->push_back("Could there be anything more to life after he returns?");
    this->chat_lines->push_back("Pray to the God you think is above, the one who will save you and show you his love. But now you are mine, dismembered and rotten, dug in your grave"); //RF, sorta
    this->chat_lines->push_back("Older things are always better."); //RF, sorta
    this->chat_lines->push_back("Consume not the orb, respect not your responsibilities; trust in your instincts and the candies.");
}

Civilian::~Civilian()
{
    delete this->chat_lines;
}

std::string Civilian::talk_general_topic()
{
    int index = TCODRandom::getInstance()->getInt(0, this->chat_lines->size()-1);
    return this->chat_lines->at(index);
};
