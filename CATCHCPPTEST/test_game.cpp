#include "stdafx.h"
#include "catch.hpp"

#include <iostream>

#include "game.h"
#include "Person.h"


TEST_CASE("Game is being tested", "[game]"){

    Game* test_game = new Game;
    // test_game->buildworld();
    // test_game->current_map = test_game->world;

    SECTION("create_person") {
        std::string name = "test pers";
        int age = 21;
        int x = 11;
        int y = 11;
        char repr = '@';
        std::string pet_name = "the pet";
        Person* test_person;
        test_person = test_game->create_person( name, age, x, y, repr,
                pet_name);
        REQUIRE(test_person->name == "test pers");
        REQUIRE(test_person->age == 21);
        REQUIRE(test_person->x == 11);
        REQUIRE(test_person->y == 11);
        REQUIRE(test_person->representation->repr == '@');
        REQUIRE(test_person->pet->name == "the pet");


    };

};



