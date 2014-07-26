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

    TCODParserStruct* rngseedsSettingsStruct = parser->newStructure("rng_seeds");
    rngseedsSettingsStruct->addProperty("spawning", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("spawning_ratio", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("item_spawn", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("linear", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("event", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("stat_rolls", TCOD_TYPE_INT, false);
    rngseedsSettingsStruct->addProperty("dungeon_builder", TCOD_TYPE_INT, false);

    // std::cout << "NAME >>" << get_computer_name() << " << " << std::endl<< std::endl<< std::endl<< std::endl;
    std::string path;
    // if (false)
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

int Parser::get_spawning_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.spawning");
};

int Parser::get_spawning_ratio_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.spawning_ratio");
};

int Parser::get_item_spawn_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.item_spawn");
};

int Parser::get_linear_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.linear");
};

int Parser::get_event_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.event");
};

int Parser::get_stat_rolls_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.stat_rolls");
};

int Parser::get_dungeon_builder_rng_seed()
{
    return this->tcod_parser->getIntProperty("rng_seeds.dungeon_builder");
};

