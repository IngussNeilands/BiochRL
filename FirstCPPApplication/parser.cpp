#include "stdafx.h"
#include "parser.h"


Parser::Parser()
{
    this->tcod_parser = this->init_parser();
};

Parser::~Parser()
{
    delete this->tcod_parser;
};

TCODParser* Parser::init_parser()
{
    TCODParser* parser = new TCODParser;
    TCODParserStruct* generalSettingsStruct = parser->newStructure("general");
    generalSettingsStruct->addProperty("enable_music", TCOD_TYPE_BOOL, true);
    generalSettingsStruct->addProperty("music_volume", TCOD_TYPE_FLOAT, true);
    generalSettingsStruct->addProperty("preferred_class_type", TCOD_TYPE_INT, true);

	// std::cout << "NAME >>" << get_computer_name() << " << " << std::endl<< std::endl<< std::endl<< std::endl;
    std::string path;
    if (get_computer_name() == "ADMIN-PC") //hack to make it load the debug one on my PC
    {
         path = std::string(get_config_path()+"config_debug.txt");
    }
    else
    {
         path = std::string(get_config_path()+"config.txt");
    };
    parser->run(path.c_str(), NULL);

    return parser;
};

bool Parser::get_enable_music()
{
    return this->tcod_parser->getBoolProperty("general.enable_music");
};

float Parser::get_music_volume()
{
    return this->tcod_parser->getFloatProperty("general.music_volume");
};

ClassType Parser::get_preferred_class_type()
{
    int type = this->tcod_parser->getIntProperty("general.preferred_class_type");
    return (ClassType)type;
};
